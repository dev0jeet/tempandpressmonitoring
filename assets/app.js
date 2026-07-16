// SPDX-FileCopyrightText: Copyright (C) Arduino s.r.l. and/or its affiliated companies
//
// SPDX-License-Identifier: MPL-2.0

document.addEventListener("DOMContentLoaded", () => {

    const tempValue = document.getElementById("tempValue");
    const pressValue = document.getElementById("pressValue");
    const tempCard = document.getElementById("tempCard");
    const pressCard = document.getElementById("pressCard");
    const tempThresholdInput = document.getElementById("tempThreshold");
    const pressThresholdInput = document.getElementById("pressThreshold");
    const intervalInput = document.getElementById("intervalInput");
    const status = document.getElementById("status");
    const errorContainer = document.getElementById("error-container");

    const MAX_DATA_POINTS = 20;

    // Chart configs
    const commonChartOptions = {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            x: { grid: { color: "#30363d" }, ticks: { color: "#8b949e", maxRotation: 0 } },
            y: { grid: { color: "#30363d" }, ticks: { color: "#8b949e" } }
        },
        plugins: { legend: { display: false } }
    };

    const tempChart = new Chart(document.getElementById("tempChart").getContext("2d"), {
        type: "line",
        data: { labels: [], datasets: [{ data: [], borderColor: "#ff7b72", backgroundColor: "rgba(255, 123, 114, 0.1)", borderWidth: 2, fill: true, tension: 0.3 }] },
        options: commonChartOptions
    });

    const pressChart = new Chart(document.getElementById("pressChart").getContext("2d"), {
        type: "line",
        data: { labels: [], datasets: [{ data: [], borderColor: "#58a6ff", backgroundColor: "rgba(88, 166, 255, 0.1)", borderWidth: 2, fill: true, tension: 0.3 }] },
        options: commonChartOptions
    });

    const ui = new WebUI();
    ui.on_connect(onUIConnected);
    ui.on_disconnect(onUIDisconnected);
    ui.on_message("sensor_data_update", handleSensorData);

    function onUIConnected() {
        if (errorContainer) errorContainer.style.display = "none";
        status.innerText = "Connected - Active";
        ui.send_message("get_initial_state");
    }

    function onUIDisconnected() {
        status.innerText = "Disconnected";
        errorContainer.style.display = "block";
        errorContainer.textContent = "Connection lost to board.";
    }

    function handleSensorData(data) {
        if (!data) return;
        const timeLabel = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
        const maxTempAlert = parseFloat(tempThresholdInput.value) || 30.0;
        const minPressAlert = parseFloat(pressThresholdInput.value) || 980.0;

        if (data.temperature !== undefined) {
            const tempVal = parseFloat(data.temperature.toFixed(2));
            tempValue.innerText = `${tempVal} °C`;
            tempCard.classList.toggle("alert", tempVal >= maxTempAlert);
            tempValue.classList.toggle("alert", tempVal >= maxTempAlert);
            updateChart(tempChart, timeLabel, tempVal);
        }

        if (data.pressure !== undefined) {
            const pressVal = parseFloat(data.pressure.toFixed(2));
            pressValue.innerText = `${pressVal} hPa`;
            pressCard.classList.toggle("alert", pressVal <= minPressAlert);
            pressValue.classList.toggle("alert", pressVal <= minPressAlert);
            updateChart(pressChart, timeLabel, pressVal);
        }
    }

    function updateChart(chart, label, value) {
        chart.data.labels.push(label);
        chart.data.datasets[0].data.push(value);
        if (chart.data.labels.length > MAX_DATA_POINTS) {
            chart.data.labels.shift();
            chart.data.datasets[0].data.shift();
        }
        chart.update();
    }

    // Handle interval changes
    intervalInput.addEventListener("change", () => {
        let val = parseInt(intervalInput.value, 10) || 2;
        if (val < 1) val = 1;
        ui.send_message("update_sampling_interval", { interval: val });
    });

});