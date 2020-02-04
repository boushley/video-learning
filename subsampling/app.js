setupDropArea()

const sourceCanvas = document.getElementById('source-canvas')
const sourceContext = sourceCanvas.getContext('2d')

function drawToCanvas(file) {
  const img = new Image()
  img.onload = () => {
    sourceContext.drawImage(img, 0, 0)
    URL.revokeObjectURL(img.src)
  }

  img.src = URL.createObjectURL(file)
}

function setupDropArea () {
  const dropArea = document.getElementById('drop-area')

  dropArea.addEventListener('dragenter', highlight, false)
  dropArea.addEventListener('dragover', highlight, false)
  dropArea.addEventListener('dragleave', unhighlight, false)
  dropArea.addEventListener('drop', (event) => {
    unhighlight(event)
    event.preventDefault()
    for (let i = 0; i < event.dataTransfer.items.length; i++) {
      if (event.dataTransfer.items[i].kind === 'file') {
        const file = event.dataTransfer.items[i].getAsFile();
        console.log('... file[' + i + '].name = ' + file.name);
        drawToCanvas(file);
      }
    }
  }, false)
  function highlight(e) {
    dropArea.classList.add('highlight')
    e.preventDefault()
  }
  function unhighlight(e) {
    dropArea.classList.remove('highlight')
    e.preventDefault()
  }
}
