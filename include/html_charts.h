// Auto-generated from charts.html
// Do not edit manually - regenerate with: node convert_html.js

#ifndef HTML_CHARTS_H
#define HTML_CHARTS_H

#include <Arduino.h>

const char HTML_CHARTS[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
    <title>Dual Zone Fridge Controller - Temperature Logs</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
  background: linear-gradient(135deg, #0f0f23 0%, #1a1a2e 50%, #16213e 100%);
  color: #ffffff;
  min-height: 100vh;
  line-height: 1.6;
}

.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 2rem;
}

.header {
  text-align: center;
  margin-bottom: 2rem;
}

.header h1 {
  font-size: 2.5rem;
  font-weight: 700;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  margin-bottom: 0.5rem;
}

.nav-bar {
  margin-bottom: 2rem;
  text-align: center;
}

.btn {
  display: inline-block;
  padding: 0.5rem 1rem;
  border-radius: 8px;
  font-weight: 500;
  text-decoration: none;
  border: none;
  cursor: pointer;
  transition: all 0.3s ease;
  text-transform: uppercase;
  letter-spacing: 0.025em;
  background: rgba(59, 130, 246, 0.1);
  color: #60a5fa;
  margin: 0 0.5rem;
}

.btn:hover, .btn:focus {
  background: rgba(59, 130, 246, 0.2);
}

.charts-section {
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 16px;
  padding: 1.5rem;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
  transition: all 0.3s ease;
}

.charts-section:hover {
  transform: translateY(-2px);
  border-color: rgba(255, 255, 255, 0.2);
  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.4);
}

.advanced-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 1.5rem;
}

.advanced-title {
  font-size: 1.5rem;
  font-weight: 600;
  color: #e2e8f0;
}

.chart-container {
  position: relative;
  height: 400px;
  margin-bottom: 1rem;
}

.chart-controls {
  display: flex;
  gap: 1rem;
  align-items: center;
  flex-wrap: wrap;
}

.control-group {
  display: flex;
  flex-direction: column;
}

.control-label {
  font-size: 0.875rem;
  font-weight: 500;
  color: #cbd5e1;
  margin-bottom: 0.5rem;
  text-transform: uppercase;
  letter-spacing: 0.025em;
}

.control-select {
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 8px;
  padding: 0.75rem;
  color: #ffffff;
  font-size: 0.875rem;
  transition: all 0.3s ease;
  cursor: pointer;
}

.control-select:focus {
  outline: none;
  border-color: #3b82f6;
  box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1);
}

.control-select:hover {
  border-color: rgba(255, 255, 255, 0.2);
}

.btn-secondary {
  background: rgba(59, 130, 246, 0.1);
  color: #60a5fa;
  border: 1px solid rgba(59, 130, 246, 0.3);
}

.btn-secondary:hover {
  background: rgba(59, 130, 246, 0.2);
  border-color: rgba(59, 130, 246, 0.5);
}

@media (max-width: 768px) {
  .container {
    padding: 1rem;
  }

  .header h1 {
    font-size: 2rem;
  }

  .chart-container {
    height: 300px;
  }

  .chart-controls {
    flex-direction: column;
    align-items: stretch;
  }

  .nav-bar .btn {
    margin-bottom: 0.5rem;
  }
}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Dual Zone Fridge Controller</h1>
            <p>Temperature Logs</p>
        </div>

        <div class="nav-bar">
            <a class="btn" href="/">Menu</a>
            <a class="btn" href="/basic">Dashboard</a>
            <a class="btn" href="/manual">Manual Controls</a>
            <a class="btn" href="/charts">Temperature Logs</a>
            <a class="btn" href="/settings">Settings</a>
        </div>

        <div class="charts-section">
            <div class="advanced-header">
                <h3 class="advanced-title">Temperature Charts</h3>
                <button class="btn btn-secondary" onclick="loadChartData()">
                    Load Data
                </button>
            </div>

            <div class="chart-container">
                <canvas id="temperatureChart" width="400" height="200"></canvas>
            </div>

            <div class="chart-controls">
                <div class="control-group">
                    <label class="control-label">Chart Time Range</label>
                    <select class="control-select" id="chartRange" onchange="loadChartData()">
                        <option value="1">Last Hour</option>
                        <option value="6">Last 6 Hours</option>
                        <option value="24">Last 24 Hours</option>
                        <option value="168">Last 7 Days</option>
                    </select>
                </div>
                <div class="control-group">
                    <label class="control-label">Data Points</label>
                    <span id="dataPoints">0 points loaded</span>
                </div>
            </div>
        </div>
    </div>

    <script>
        let temperatureChart = null;

        function loadChartData() {
            const range = document.getElementById('chartRange').value;
            const dataPointsElement = document.getElementById('dataPoints');

            dataPointsElement.textContent = 'Loading...';

            fetch('/api/logs')
                .then(response => response.json())
                .then(data => {
                    if (data.length === 0) {
                        dataPointsElement.textContent = 'No data available';
                        return;
                    }

                    // Filter data based on selected time range (hours)
                    const now = Date.now() / 1000; // Current time in seconds
                    const rangeSeconds = range * 3600; // Convert hours to seconds
                    const filteredData = data.filter(point => {
                        return (now - point.timestamp) <= rangeSeconds;
                    });

                    if (filteredData.length === 0) {
                        dataPointsElement.textContent = 'No data in selected range';
                        return;
                    }

                    // Prepare data for Chart.js
                    const labels = filteredData.map(point => {
                        const date = new Date(point.timestamp * 1000);
                        // Only show date for 7-day range, otherwise just time
                        if (range === '168') {
                            // 7 days - show short date and time
                            return date.toLocaleDateString(undefined, { month: 'short', day: 'numeric' }) + ' ' + 
                                   date.toLocaleTimeString(undefined, { hour: '2-digit', minute: '2-digit' });
                        } else {
                            // Shorter ranges - show only time
                            return date.toLocaleTimeString(undefined, { hour: '2-digit', minute: '2-digit', second: '2-digit' });
                        }
                    });

                    const leftTemps = filteredData.map(point => point.leftTemp);
                    const rightTemps = filteredData.map(point => point.rightTemp);
                    const leftSetpoints = filteredData.map(point => point.setpointLeft);
                    const rightSetpoints = filteredData.map(point => point.setpointRight);

                    // Update chart
                    updateChart(labels, leftTemps, rightTemps, leftSetpoints, rightSetpoints);
                    dataPointsElement.textContent = filteredData.length + ' points loaded';
                })
                .catch(error => {
                    console.error('Error loading chart data:', error);
                    dataPointsElement.textContent = 'Error loading data';
                });
        }

        function updateChart(labels, leftTemps, rightTemps, leftSetpoints, rightSetpoints) {
            const ctx = document.getElementById('temperatureChart').getContext('2d');

            // Destroy existing chart if it exists
            if (temperatureChart) {
                temperatureChart.destroy();
            }

            temperatureChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [
                        {
                            label: 'Left Zone Temperature (°C)',
                            data: leftTemps,
                            borderColor: 'rgba(59, 130, 246, 1)',
                            backgroundColor: 'rgba(59, 130, 246, 0.1)',
                            borderWidth: 2,
                            fill: false,
                            tension: 0.4,
                            pointRadius: 2,
                            pointHoverRadius: 4
                        },
                        {
                            label: 'Right Zone Temperature (°C)',
                            data: rightTemps,
                            borderColor: 'rgba(16, 185, 129, 1)',
                            backgroundColor: 'rgba(16, 185, 129, 0.1)',
                            borderWidth: 2,
                            fill: false,
                            tension: 0.4,
                            pointRadius: 2,
                            pointHoverRadius: 4
                        },
                        {
                            label: 'Left Setpoint (°C)',
                            data: leftSetpoints,
                            borderColor: 'rgba(59, 130, 246, 0.5)',
                            backgroundColor: 'rgba(59, 130, 246, 0.05)',
                            borderWidth: 1,
                            borderDash: [5, 5],
                            fill: false,
                            pointRadius: 0,
                            tension: 0.1
                        },
                        {
                            label: 'Right Setpoint (°C)',
                            data: rightSetpoints,
                            borderColor: 'rgba(16, 185, 129, 0.5)',
                            backgroundColor: 'rgba(16, 185, 129, 0.05)',
                            borderWidth: 1,
                            borderDash: [5, 5],
                            fill: false,
                            pointRadius: 0,
                            tension: 0.1
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    interaction: {
                        mode: 'index',
                        intersect: false
                    },
                    plugins: {
                        title: {
                            display: true,
                            text: 'Temperature History',
                            color: '#ffffff',
                            font: {
                                size: 16,
                                weight: 'bold'
                            }
                        },
                        legend: {
                            labels: {
                                color: '#cbd5e1',
                                usePointStyle: true,
                                pointStyle: 'line'
                            }
                        },
                        tooltip: {
                            backgroundColor: 'rgba(0, 0, 0, 0.8)',
                            titleColor: '#ffffff',
                            bodyColor: '#ffffff',
                            borderColor: 'rgba(255, 255, 255, 0.1)',
                            borderWidth: 1
                        }
                    },
                    scales: {
                        x: {
                            display: true,
                            title: {
                                display: true,
                                text: 'Time',
                                color: '#cbd5e1'
                            },
                            ticks: {
                                color: '#cbd5e1',
                                maxTicksLimit: 10
                            },
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            }
                        },
                        y: {
                            display: true,
                            title: {
                                display: true,
                                text: 'Temperature (°C)',
                                color: '#cbd5e1'
                            },
                            ticks: {
                                color: '#cbd5e1'
                            },
                            grid: {
                                color: 'rgba(255, 255, 255, 0.1)'
                            }
                        }
                    },
                    elements: {
                        point: {
                            hoverBorderWidth: 2
                        }
                    }
                }
            });
        }

        // Initial load
        loadChartData();
    </script>
</body>
</html>

)====" ;

#endif // HTML_CHARTS_H
