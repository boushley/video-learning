const els = {
  dropArea: document.getElementById('drop-area'),
  lumaSamples: document.getElementById('luma-samples'),
  chromaSamples: document.getElementById('chroma-samples'),
  chromaDisplay: document.getElementById('chroma-display'),
  subsampledDisplay: document.getElementById('subsampled-display'),
  blendingMode: document.getElementById('blending-mode'),

  zoomSourceCanvas: document.getElementById('zoom-source-canvas'),
  sourceCanvas: document.getElementById('source-canvas'),
  yPlaneCanvas: document.getElementById('y-plane-canvas'),
  uPlaneCanvas: document.getElementById('u-plane-canvas'),
  vPlaneCanvas: document.getElementById('v-plane-canvas'),
  zoomResultCanvas: document.getElementById('zoom-result-canvas'),
  resultCanvas: document.getElementById('result-canvas'),

  zoomSourceContext: document.getElementById('zoom-source-canvas').getContext('2d'),
  sourceContext: document.getElementById('source-canvas').getContext('2d'),
  yPlaneContext: document.getElementById('y-plane-canvas').getContext('2d'),
  uPlaneContext: document.getElementById('u-plane-canvas').getContext('2d'),
  vPlaneContext: document.getElementById('v-plane-canvas').getContext('2d'),
  zoomResultContext: document.getElementById('zoom-result-canvas').getContext('2d'),
  resultContext: document.getElementById('result-canvas').getContext('2d'),
}

const state = {}
let sourceImage = {}

const subsampleMap = {
  '4:4': {
    xScale: 1,
    yScale: 1,
  },
  '4:0': {
    xScale: 1,
    yScale: 0.5,
  },
  '2:2': {
    xScale: 0.5,
    yScale: 1,
  },
  '2:0': {
    xScale: 0.5,
    yScale: 0.5,
  },
  '1:1': {
    xScale: 0.25,
    yScale: 1,
  },
  '1:0': {
    xScale: 0.25,
    yScale: 0.5,
  }
}

setupControls()

function getYUV({r, g, b}) {
  y = (0.299 * r) + (0.587 * g) + (0.114 * b)
  u = (-0.147 * r) - (0.289 * g) + (0.436 * b)
  v = (0.615 * r) - (0.515 * g) - (0.100 * b)

  return {y, u, v}
}

function getRGB({y, u, v}) {
  r = y + (1.403 * v)
  g = y - (0.344 * u) - (0.714 * v)
  b = y + (1.770 * u)

  return {r, g, b}
}

function renderPlane(context, plane, samples, toRGB) {
  // TODO Implement Subsampled Display Mode
  let planeData
  if (state.subsampledDisplay === 'scaled') {
    planeData = context.createImageData(Math.ceil(sourceImage.width * samples.xScale), Math.ceil(sourceImage.height * samples.yScale))
  } else if (state.subsampledDisplay === 'stretched') {
    planeData = context.createImageData(sourceImage.width, sourceImage.height)
  } else {
    throw new Exception('Unhandled subsample display type: ' + state.subsampleDisplay)
  }

  for (let i = 0; i < planeData.width; i++) {
    for (let j = 0; j < planeData.height; j++) {
      const pixelNumber = (j * planeData.width) + i
      const pixelStart = pixelNumber * 4

      const y = plane[pixelNumber]
      const {r, g, b} = toRGB(y)

      planeData.data[pixelStart] = r
      planeData.data[pixelStart+1] = g
      planeData.data[pixelStart+2] = b
      planeData.data[pixelStart+3] = 255
    }
  }
  context.putImageData(planeData, 0, 0)
}

function renderRecombination() {
  const planeData = els.resultContext.createImageData(sourceImage.width, sourceImage.height)
  for (let i = 0; i < sourceImage.width; i++) {
    for (let j = 0; j < sourceImage.height; j++) {
      const pixelStart = ((j * sourceImage.width) + i) * 4

      const yuv = {
        y: sourceImage.yPlane[pixelNumber(state.lumaSamples, i, j)],
        u: sourceImage.uPlane[pixelNumber(state.chromaSamples, i, j)],
        v: sourceImage.vPlane[pixelNumber(state.chromaSamples, i, j)],
      }
      const {r, g, b} = getRGB(yuv)

      planeData.data[pixelStart] = r
      planeData.data[pixelStart+1] = g
      planeData.data[pixelStart+2] = b
      planeData.data[pixelStart+3] = 255
    }
  }
  els.resultContext.putImageData(planeData, 0, 0)
}

function renderOutputs() {
  const chromaAsGray = state.chromaDisplay === 'grayscale'
  const grayScale = c => getRGB({y: c + 127, u: 0, v: 0})
  const uColorizer = chromaAsGray ? grayScale : u => getRGB({ y: 128, u, v: -64 })
  const vColorizer = chromaAsGray ? grayScale : v => getRGB({ y: 128, u: -64, v })
  renderPlane(els.yPlaneContext, sourceImage.yPlane, state.lumaSamples, y => getRGB({ y, u: 0, v: 0 }))
  renderPlane(els.uPlaneContext, sourceImage.uPlane, state.chromaSamples, uColorizer)
  renderPlane(els.vPlaneContext, sourceImage.vPlane, state.chromaSamples, vColorizer)
  renderRecombination()
}

function pixelCount(subsample) {
  return (sourceImage.width * subsample.xScale) * (sourceImage.height * subsample.yScale)
}

function pixelNumber(subsample, x, y) {
  return (Math.floor(y * subsample.yScale) * Math.floor(sourceImage.width * subsample.xScale)) + Math.floor(x * subsample.xScale)
}

function adjustCanvasSize(el, subsample) {
  el.width = Math.ceil(sourceImage.width * subsample.xScale)
  el.height = Math.ceil(sourceImage.height * subsample.yScale)
}

function adjustCanvasSizes() {
  adjustCanvasSize(els.resultCanvas, subsampleMap['4:4'])
  adjustCanvasSize(els.yPlaneCanvas, state.lumaSamples)
  adjustCanvasSize(els.uPlaneCanvas, state.chromaSamples)
  adjustCanvasSize(els.vPlaneCanvas, state.chromaSamples)
}

function processData() {
  if (!state.lumaSamples || !sourceImage.width || !sourceImage.height) {
    return
  }

  adjustCanvasSizes()
  sourceImage.imageData = els.sourceContext.getImageData(0, 0, sourceImage.width, sourceImage.height)

  sourceImage.yPlane = new Uint8Array(pixelCount(state.lumaSamples))
  sourceImage.uPlane = new Int8Array(pixelCount(state.chromaSamples))
  sourceImage.vPlane = new Int8Array(pixelCount(state.chromaSamples))

  for (let i = 0; i < sourceImage.width; i++) {
    for (let j = 0; j < sourceImage.height; j++) {
      const pixelStart = ((j * sourceImage.width) + i) * 4
      const pixel = {
        r: sourceImage.imageData.data[pixelStart],
        g: sourceImage.imageData.data[pixelStart + 1],
        b: sourceImage.imageData.data[pixelStart + 2],
      }
      const {y, u, v} = getYUV(pixel)
      sourceImage.yPlane[pixelNumber(state.lumaSamples, i, j)] = y
      sourceImage.uPlane[pixelNumber(state.chromaSamples, i, j)] = u
      sourceImage.vPlane[pixelNumber(state.chromaSamples, i, j)] = v
    }
  }

  renderOutputs()
}

function drawFileToCanvas(file) {
  const url = URL.createObjectURL(file)
  drawUrlToCanvas(url, () => URL.revokeObjectURL(url))
}

function drawUrlToCanvas(url, cb) {
  els.dropArea.classList.add('hidden')
  const img = new Image()
  img.onload = () => {
    if (cb) {
      cb()
    }
    sourceImage.width = img.naturalWidth
    sourceImage.height = img.naturalHeight
    adjustCanvasSize(els.sourceCanvas, subsampleMap['4:4'])
    els.sourceContext.drawImage(img, 0, 0)

    processData()
  }

  img.src = url
}

function setupControls() {
  els.lumaSamples.addEventListener('change', saveControlState)
  els.chromaSamples.addEventListener('change', saveControlState)
  els.chromaDisplay.addEventListener('change', saveControlState)
  els.subsampledDisplay.addEventListener('change', saveControlState)
  els.blendingMode.addEventListener('change', saveControlState)

  setupDropArea()
  saveControlState()
  setupZoom()
}

function saveControlState() {
    state.lumaSamples = subsampleMap[els.lumaSamples.value]
    state.chromaSamples = subsampleMap[els.chromaSamples.value]
    state.chromaDisplay = els.chromaDisplay.value
    state.subsampledDisplay = els.subsampledDisplay.value
    state.blendingMode = els.blendingMode.value
    processData()
}

function setupDropArea () {
  document.body.addEventListener('dragenter', highlight)
  document.body.addEventListener('dragover', highlight)
  els.dropArea.addEventListener('dragenter', highlight, false)
  els.dropArea.addEventListener('dragover', highlight, false)
  els.dropArea.addEventListener('dragleave', unhighlight, false)
  els.dropArea.addEventListener('drop', (event) => {
    unhighlight(event)
    event.preventDefault()
    for (let i = 0; i < event.dataTransfer.items.length; i++) {
      if (event.dataTransfer.items[i].kind === 'file') {
        const file = event.dataTransfer.items[i].getAsFile();
        sourceImage.file = file
        drawFileToCanvas(file);
        break;
      }
    }
  }, false)
  function highlight(e) {
    els.dropArea.classList.add('highlight')
    e.preventDefault()
  }
  function unhighlight(e) {
    els.dropArea.classList.remove('highlight')
    e.preventDefault()
  }
}

function setupZoom() {
  els.sourceCanvas.addEventListener('mousemove', updateZoom)
  els.resultCanvas.addEventListener('mousemove', updateZoom)
}

function updateZoom(e) {
  const zoomDimension = 50
  const half = zoomDimension / 2
  const elRect = e.target.getBoundingClientRect()
  const copyX = clamp(0, e.clientX - elRect.x - half, e.target.width - half)
  const copyY = clamp(0, e.clientY - elRect.y - half, e.target.height - half)

  renderZoom(copyX, copyY, zoomDimension, zoomDimension, els.sourceContext, els.zoomSourceContext)
  renderZoom(copyX, copyY, zoomDimension, zoomDimension, els.resultContext, els.zoomResultContext)
}

function renderZoom(x, y, width, height, source, output) {
  const sourceData = source.getImageData(x, y, width, height)
  output.putImageData(sourceData, 0, 0)

}

function clamp(low, val, high) {
  return Math.max(Math.min(val, high), low)
}

drawUrlToCanvas('../test-assets/small-tears-of-steel-frame-00829.png')
