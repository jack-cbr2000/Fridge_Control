// Auto-generated from index.html
// Do not edit manually - regenerate with: node convert_html.js

#ifndef HTML_INDEX_H
#define HTML_INDEX_H

#include <Arduino.h>

const char HTML_INDEX[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
    <title>Dual Zone Fridge Controller - Menu</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap" rel="stylesheet">
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
  max-width: 800px;
  margin: 0 auto;
  padding: 2rem;
  text-align: center;
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

.menu-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 2rem;
  margin-top: 3rem;
}

.menu-card {
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 16px;
  padding: 2rem;
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
  transition: all 0.3s ease;
  text-decoration: none;
  color: inherit;
  display: block;
}

.menu-card:hover {
  transform: translateY(-5px);
  border-color: rgba(255, 255, 255, 0.2);
  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.4);
}

.menu-title {
  font-size: 1.5rem;
  font-weight: 600;
  margin-bottom: 1rem;
  color: #e2e8f0;
}

.menu-description {
  font-size: 0.9rem;
  opacity: 0.8;
  margin-bottom: 1.5rem;
}

.btn {
  display: inline-block;
  padding: 0.75rem 1.5rem;
  border-radius: 12px;
  font-weight: 500;
  text-decoration: none;
  border: none;
  cursor: pointer;
  transition: all 0.3s ease;
  text-transform: uppercase;
  letter-spacing: 0.025em;
  background: linear-gradient(135deg, #22c55e 0%, #16a34a 100%);
  color: white;
  box-shadow: 0 4px 15px rgba(34, 197, 94, 0.3);
}

.btn:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(34, 197, 94, 0.4);
}

@media (max-width: 768px) {
  .container {
    padding: 1rem;
  }

  .header h1 {
    font-size: 2rem;
  }

  .menu-grid {
    grid-template-columns: 1fr;
  }
}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Dual Zone Fridge Controller</h1>
            <div class="nav-bar">
                <a class="btn" href="/">Menu</a>
                <a class="btn" href="/basic">Dashboard</a>
                <a class="btn" href="/manual">Manual Controls</a>
                <a class="btn" href="/charts">Temperature Logs</a>
                <a class="btn" href="/settings">Settings</a>
            </div>
        </div>

        <div class="menu-grid">
            <a href="/basic" class="menu-card">
                <div class="menu-title">Dashboard</div>
                <div class="menu-description">View current status, temperatures, setpoints, and zone control</div>
                <div class="btn">Open Dashboard</div>
            </a>

            <a href="/manual" class="menu-card">
                <div class="menu-title">Manual Controls</div>
                <div class="menu-description">Manual control for compressor and solenoid valves</div>
                <div class="btn">Manual Control</div>
            </a>

            <a href="/charts" class="menu-card">
                <div class="menu-title">Temperature Logs</div>
                <div class="menu-description">View temperature charts and historical data</div>
                <div class="btn">View Charts</div>
            </a>

            <a href="/settings" class="menu-card">
                <div class="menu-title">Settings</div>
                <div class="menu-description">Configure hysteresis, run times, and other advanced options</div>
                <div class="btn">Configure</div>
            </a>
        </div>
    </div>
</body>
</html>

)====" ;

#endif // HTML_INDEX_H
