function setLED(on) {
  var data = 'led=' + encodeURIComponent(Number(on));
  fetch('/led', {
    method: 'POST',
    body: data,
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded'
    }
  });
}

document.querySelector("#led-on").onclick = function () {
  setLED(true);
};

document.querySelector("#led-off").onclick = function () {
  setLED(false);
};
