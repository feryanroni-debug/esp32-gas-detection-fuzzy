// charts.js - Inisialisasi dan pembaruan grafik telemetri real-time

let sensorChart = null;
const MAX_DATA_POINTS = 20;

function initTelemetryChart() {
    const ctx = document.getElementById('telemetryChart');
    if (!ctx) return;

    sensorChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'Gas LPG (ppm)',
                    data: [],
                    borderColor: '#f97316', // Orange
                    backgroundColor: 'rgba(249, 115, 22, 0.1)',
                    borderWidth: 2,
                    tension: 0.3,
                    fill: true,
                    yAxisID: 'yGas'
                },
                {
                    label: 'Asap (ppm)',
                    data: [],
                    borderColor: '#64748b', // Slate
                    backgroundColor: 'rgba(100, 116, 139, 0.1)',
                    borderWidth: 2,
                    tension: 0.3,
                    fill: false,
                    yAxisID: 'yGas'
                },
                {
                    label: 'Suhu (°C)',
                    data: [],
                    borderColor: '#06b6d4', // Cyan
                    borderWidth: 2,
                    borderDash: [5, 5],
                    fill: false,
                    yAxisID: 'yTemp'
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            interaction: {
                mode: 'index',
                intersect: false,
            },
            scales: {
                x: {
                    grid: { color: 'rgba(255, 255, 255, 0.05)' },
                    ticks: { color: '#94a3b8' }
                },
                yGas: {
                    type: 'linear',
                    display: true,
                    position: 'left',
                    title: { display: true, text: 'Gas / Asap (ppm)', color: '#94a3b8' },
                    grid: { color: 'rgba(255, 255, 255, 0.05)' },
                    ticks: { color: '#94a3b8' },
                    suggestedMin: 0,
                    suggestedMax: 2000
                },
                yTemp: {
                    type: 'linear',
                    display: true,
                    position: 'right',
                    title: { display: true, text: 'Suhu (°C)', color: '#06b6d4' },
                    grid: { drawOnChartArea: false },
                    ticks: { color: '#06b6d4' },
                    suggestedMin: 20,
                    suggestedMax: 50
                }
            },
            plugins: {
                legend: {
                    labels: { color: '#e2e8f0', usePointStyle: true }
                }
            }
        }
    });
}

function updateChartData(timestampStr, gasVal, smokeVal, tempVal) {
    if (!sensorChart) return;

    const timeLabel = new Date(timestampStr).toLocaleTimeString();

    sensorChart.data.labels.push(timeLabel);
    sensorChart.data.datasets[0].data.push(gasVal);
    sensorChart.data.datasets[1].data.push(smokeVal);
    sensorChart.data.datasets[2].data.push(tempVal);

    if (sensorChart.data.labels.length > MAX_DATA_POINTS) {
        sensorChart.data.labels.shift();
        sensorChart.data.datasets.forEach(dataset => dataset.data.shift());
    }

    sensorChart.update('none'); // Update cepat tanpa animasi berulang
}

window.initTelemetryChart = initTelemetryChart;
window.updateChartData = updateChartData;
