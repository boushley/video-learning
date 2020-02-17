const els = {
  dropArea: document.getElementById('drop-area'),
  sourceCanvas: document.getElementById('source-canvas'),
  sourceContext: document.getElementById('source-canvas').getContext('2d'),
  yPlaneCanvas: document.getElementById('y-plane-canvas'),
  yPlaneContext: document.getElementById('y-plane-canvas').getContext('2d'),
  uPlaneCanvas: document.getElementById('u-plane-canvas'),
  uPlaneContext: document.getElementById('u-plane-canvas').getContext('2d'),
  vPlaneCanvas: document.getElementById('v-plane-canvas'),
  vPlaneContext: document.getElementById('v-plane-canvas').getContext('2d'),
  resultCanvas: document.getElementById('result-canvas'),
  resultContext: document.getElementById('result-canvas').getContext('2d'),
}

let sourceImage = {}

setupDropArea()

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

function renderPlane(context, plane, toRGB) {
  const planeData = context.createImageData(sourceImage.width, sourceImage.height)
  for (let i = 0; i < sourceImage.width; i++) {
    for (let j = 0; j < sourceImage.height; j++) {
      const pixelNumber = (j * sourceImage.width) + i
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
      const pixelNumber = (j * sourceImage.width) + i
      const pixelStart = pixelNumber * 4

      const yuv = {
        y: sourceImage.yPlane[pixelNumber],
        u: sourceImage.uPlane[pixelNumber],
        v: sourceImage.vPlane[pixelNumber],
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
  renderPlane(els.yPlaneContext, sourceImage.yPlane, y => getRGB({ y, u: 0, v: 0 }))
  renderPlane(els.uPlaneContext, sourceImage.uPlane, u => getRGB({ y: 128, u, v: -67 }))
  renderPlane(els.vPlaneContext, sourceImage.vPlane, v => getRGB({ y: 128, u: -67, v }))
  renderRecombination()
}

function dataReady() {
  sourceImage.imageData = els.sourceContext.getImageData(0, 0, sourceImage.width, sourceImage.height)

  const pixelCount = sourceImage.width * sourceImage.height
  sourceImage.yPlane = new Uint8Array(pixelCount)
  sourceImage.uPlane = new Int8Array(pixelCount)
  sourceImage.vPlane = new Int8Array(pixelCount)

  for (let i = 0; i < sourceImage.width; i++) {
    for (let j = 0; j < sourceImage.height; j++) {
      const pixelNumber = (j * sourceImage.width) + i
      const pixelStart = pixelNumber * 4
      const pixel = {
        r: sourceImage.imageData.data[pixelStart],
        g: sourceImage.imageData.data[pixelStart + 1],
        b: sourceImage.imageData.data[pixelStart + 2],
      }
      const {y, u, v} = getYUV(pixel)
      sourceImage.yPlane[pixelNumber] = y
      sourceImage.uPlane[pixelNumber] = u
      sourceImage.vPlane[pixelNumber] = v
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
    els.sourceContext.drawImage(img, 0, 0)

    dataReady()
  }

  img.src = url
}

function setupDropArea () {
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

drawUrlToCanvas('/test-assets/small-tears-of-steel-frame-00829.png')
