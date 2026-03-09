const bodyElement = document.body;
const distValElement = document.getElementById('dist-val');
const frontSensor = document.getElementById('front-sensor');
const rear1 = document.getElementById('rear-1');
const rear2 = document.getElementById('rear-2');
const rear3 = document.getElementById('rear-3');

function updateRadar(frontBlocked, rearDistanceCm) {
    if (rearDistanceCm >= 999) {
        distValElement.innerText = "OUT";
        distValElement.style.color = "white";
    } else {
        distValElement.innerText = (rearDistanceCm / 100).toFixed(2);
    }

    if (frontBlocked) frontSensor.classList.add('danger');
    else frontSensor.classList.remove('danger');

    rear1.className = 'sensor-arc rear-arc-1';
    rear2.className = 'sensor-arc rear-arc-2';
    rear3.className = 'sensor-arc rear-arc-3';

    if (rearDistanceCm <= 5 || frontBlocked) {
        bodyElement.style.backgroundColor = "#3a0000"; 
        distValElement.style.color = "#ff3333";
        rear1.classList.add('danger');
        rear2.classList.add('warning');
        rear3.classList.add('safe');
    } else if (rearDistanceCm <= 10) {
        bodyElement.style.backgroundColor = "#332200";
        distValElement.style.color = "#ffaa00";
        rear2.classList.add('warning');
        rear3.classList.add('safe');
    } else if (rearDistanceCm <= 15) {
        bodyElement.style.backgroundColor = "#002b18";
        distValElement.style.color = "#00ffaa";
        rear3.classList.add('safe');
    } else {
        bodyElement.style.backgroundColor = "#011936";
        distValElement.style.color = "white";
    }
}

// Fetch live data from the ESP32 every 250 milliseconds
setInterval(async () => {
    try {
        let response = await fetch('/data');
        let data = await response.json();
        updateRadar(data.frontBlocked, data.rearDistance);
    } catch (err) {
        console.error("Waiting for ESP32 connection...");
    }
}, 250);