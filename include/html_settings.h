// Auto-generated from settings.html
// Do not edit manually - regenerate with: node convert_html.js

#ifndef HTML_SETTINGS_H
#define HTML_SETTINGS_H

#include <Arduino.h>

const char HTML_SETTINGS[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Dual Zone Fridge Controller - Settings</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap" rel="stylesheet">
    <style>
*{margin:0;padding:0;box-sizing:border-box}body{font-family:'Inter',-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:linear-gradient(135deg,#0f0f23 0%,#1a1a2e 50%,#16213e 100%);color:#fff;min-height:100vh;line-height:1.6}.container{max-width:1200px;margin:0 auto;padding:2rem}.header{text-align:center;margin-bottom:2rem}.header h1{font-size:2.5rem;font-weight:700;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;margin-bottom:.5rem}.nav-bar{margin-bottom:2rem;text-align:center}.btn{display:inline-block;padding:.5rem 1rem;border-radius:8px;font-weight:500;text-decoration:none;border:none;cursor:pointer;transition:all .3s ease;text-transform:uppercase;letter-spacing:.025em;background:rgba(59,130,246,.1);color:#60a5fa;margin:0 .5rem}.btn:hover,.btn:focus{background:rgba(59,130,246,.2)}.advanced-settings{margin-top:2rem}.advanced-header{display:flex;align-items:center;justify-content:space-between;margin-bottom:1.5rem}.advanced-title{font-size:1.5rem;font-weight:600;color:#e2e8f0}.advanced-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:1.5rem}.control-group{display:flex;flex-direction:column}.control-label{font-size:.875rem;font-weight:500;color:#cbd5e1;margin-bottom:.5rem;text-transform:uppercase;letter-spacing:.025em}.control-select,.control-input{background:rgba(255,255,255,.05);border:1px solid rgba(255,255,255,.1);border-radius:8px;padding:.75rem;color:#fff;font-size:.875rem;transition:all .3s ease;width:100%}.control-select:focus,.control-input:focus{outline:none;border-color:#3b82f6;box-shadow:0 0 0 3px rgba(59,130,246,.1)}.control-select{cursor:pointer}.control-input::placeholder{color:rgba(255,255,255,.4)}.wifi-status{background:rgba(255,255,255,.05);border:1px solid rgba(255,255,255,.1);border-radius:8px;padding:1rem;margin-bottom:1.5rem}.wifi-status p{margin:.5rem 0;font-size:.875rem}.wifi-config{background:rgba(255,255,255,.05);border:1px solid rgba(255,255,255,.1);border-radius:8px;padding:1.5rem}.btn-primary{background:rgba(59,130,246,.2);color:#60a5fa;border:1px solid rgba(59,130,246,.3)}.btn-primary:hover{background:rgba(59,130,246,.3)}.btn-success{background:rgba(34,197,94,.2);color:#4ade80;border:1px solid rgba(34,197,94,.3);margin-top:1rem}.btn-success:hover{background:rgba(34,197,94,.3)}.loader{display:inline-block;width:20px;height:20px;border:2px solid rgba(255,255,255,.3);border-radius:50%;border-top-color:#3b82f6;animation:spin 1s ease-in-out infinite}@keyframes spin{to{transform:rotate(360deg)}}.network-card{background:rgba(255,255,255,.08);border:1px solid rgba(255,255,255,.15);border-radius:8px;padding:1rem;margin-bottom:.75rem;display:flex;align-items:center;justify-content:space-between;transition:all .3s ease;cursor:move}.network-card:hover{background:rgba(255,255,255,.12);border-color:rgba(255,255,255,.25)}.network-card.empty-slot{background:rgba(255,255,255,.03);border-style:dashed;border-color:rgba(255,255,255,.1);cursor:default}.network-info{flex:1;display:flex;align-items:center;gap:.75rem}.network-priority{background:rgba(59,130,246,.2);color:#60a5fa;padding:.25rem .5rem;border-radius:4px;font-weight:600;font-size:.75rem;min-width:24px;text-align:center}.network-details{display:flex;flex-direction:column;gap:.25rem}.network-ssid{font-weight:600;color:#e2e8f0;font-size:.875rem}.network-status{font-size:.75rem;color:#cbd5e1}.network-actions{display:flex;gap:.5rem}.btn-small{padding:.25rem .5rem;font-size:.75rem;background:rgba(239,68,68,.2);color:#f87171;border:1px solid rgba(239,68,68,.3)}.btn-small:hover{background:rgba(239,68,68,.3)}.drag-handle{color:rgba(255,255,255,.3);font-size:1.25rem;cursor:grab;margin-right:.5rem}@media(max-width:768px){.container{padding:1rem}.header h1{font-size:2rem}.advanced-grid{grid-template-columns:1fr}.network-card{flex-direction:column;align-items:flex-start}.network-actions{width:100%;margin-top:.5rem}.nav-bar .btn{margin-bottom:.5rem}}
    </style>
</head>
<body>
    <div class="container">
        <div class="header"><h1>Dual Zone Fridge Controller</h1><p>Settings</p></div>
        <div class="nav-bar">
            <a class="btn" href="/">Menu</a>
            <a class="btn" href="/basic">Dashboard</a>
            <a class="btn" href="/manual">Manual Controls</a>
            <a class="btn" href="/charts">Temperature Logs</a>
            <a class="btn" href="/settings">Settings</a>
        </div>
        <div class="advanced-settings">
            <div class="advanced-header"><h3 class="advanced-title">WiFi Configuration</h3></div>
            <div class="wifi-status" id="wifiStatus">
                <p><strong>Current Status:</strong> <span id="currentStatus">Loading...</span></p>
                <p><strong>Connected to:</strong> <span id="connectedSSID">-</span></p>
                <p><strong>Direct IP Access:</strong> <a href="#" id="ipAddressLink" style="color:#60a5fa;text-decoration:none;font-weight:bold;" onclick="return false;"><span id="ipAddress">-</span></a> <button onclick="copyIP()" style="background:rgba(59,130,246,.2);border:1px solid rgba(59,130,246,.3);color:#60a5fa;padding:.25rem .5rem;border-radius:4px;cursor:pointer;font-size:.75rem;margin-left:.5rem;">Copy</button></p>
                <p><strong>Easy Access URL:</strong> <a href="http://fridge.local" style="color:#60a5fa;text-decoration:none;">http://fridge.local</a></p>
            </div>
            <div class="wifi-config">
                <h4 style="margin-bottom:1rem;color:#e2e8f0;">Configured Networks</h4>
                <div id="configuredNetworks" style="margin-bottom:1.5rem;"></div>
                <hr style="border:none;border-top:1px solid rgba(255,255,255,.1);margin:1.5rem 0;">
                <h4 style="margin-bottom:1rem;color:#e2e8f0;">Add New Network</h4>
                <button class="btn btn-primary" onclick="scanNetworks()" id="scanBtn"><span id="scanBtnText">Scan Networks</span></button>
                <div id="networkList" style="display:none;margin-top:1rem;">
                    <label class="control-label">Available Networks</label>
                    <select class="control-select" id="networkSelect" onchange="selectNetwork()"><option value="">-- Select Network --</option></select>
                </div>
                <div class="control-group" style="margin-top:1rem;">
                    <label class="control-label">WiFi SSID</label>
                    <input type="text" class="control-input" id="wifiSsid" placeholder="Enter SSID">
                </div>
                <div class="control-group">
                    <label class="control-label">WiFi Password</label>
                    <input type="password" class="control-input" id="wifiPassword" placeholder="Enter password">
                </div>
                <button class="btn btn-success" onclick="addNetwork()">Add Network</button>
                <button class="btn btn-primary" onclick="saveAllNetworks()" style="margin-left:.5rem;">Save All Changes</button>
            </div>
        </div>
        <div class="advanced-settings" style="margin-top:2rem;">
            <div class="advanced-header"><h3 class="advanced-title">Advanced Settings</h3></div>
            <div class="advanced-grid">
                <div class="control-group"><label class="control-label">Hysteresis (C)</label><select class="control-select" id="hysteresis" onchange="updateConfig()"><option value="1.0">1.0</option><option value="1.5">1.5</option><option value="2.0">2.0</option><option value="2.5">2.5</option><option value="3.0">3.0</option><option value="3.5">3.5</option><option value="4.0">4.0</option><option value="4.5">4.5</option><option value="5.0">5.0</option></select></div>
                <div class="control-group"><label class="control-label">Min Run Time (min)</label><select class="control-select" id="minRunTime" onchange="updateConfig()"><option value="0.5">0.5</option><option value="1.0">1.0</option><option value="1.5">1.5</option><option value="2.0">2.0</option><option value="3.0">3.0</option><option value="5.0">5.0</option></select></div>
                <div class="control-group"><label class="control-label">Min Stop Time (min)</label><select class="control-select" id="minStopTime" onchange="updateConfig()"><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="5">5</option><option value="7">7</option><option value="10">10</option></select></div>
                <div class="control-group"><label class="control-label">Min Zone Switch (min)</label><select class="control-select" id="minZoneSwitchTime" onchange="updateConfig()"><option value="1">1</option><option value="2">2</option><option value="3">3</option><option value="5">5</option><option value="7">7</option><option value="10">10</option></select></div>
                <div class="control-group"><label class="control-label">Max Run Time (min)</label><select class="control-select" id="maxRunTime" onchange="updateConfig()"><option value="15">15</option><option value="20">20</option><option value="25">25</option><option value="30">30</option><option value="35">35</option><option value="45">45</option></select></div>
                <div class="control-group"><label class="control-label">Temp Offset (C)</label><select class="control-select" id="tempOffset" onchange="updateConfig()"><option value="-5">-5</option><option value="-3">-3</option><option value="-1">-1</option><option value="0">0</option><option value="1">1</option><option value="3">3</option><option value="5">5</option></select></div>
            </div>
        </div>
        <div class="advanced-settings" style="margin-top:2rem;">
            <div class="advanced-header"><h3 class="advanced-title">Firmware Updates</h3></div>
            <div class="wifi-config">
                <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:1rem;">
                    <div><strong>Current Version:</strong> <span id="currentVersion">Loading...</span><br><strong>Last Checked:</strong> <span id="lastChecked">Never</span></div>
                    <button class="btn btn-success" onclick="checkForUpdates()" id="checkUpdateBtn">Check for Updates</button>
                </div>
                <div style="display:flex;align-items:center;gap:1rem;margin-bottom:1rem;padding:1rem;background:rgba(255,255,255,.05);border-radius:8px;">
                    <label style="display:flex;align-items:center;gap:.75rem;cursor:pointer;flex:1;">
                        <div style="position:relative;width:50px;height:26px;">
                            <input type="checkbox" id="autoUpdatesToggle" onchange="toggleAutoUpdates()" style="opacity:0;width:0;height:0;">
                            <span id="toggleSlider" style="position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:rgba(239,68,68,.5);transition:.3s;border-radius:26px;"></span>
                            <span id="toggleKnob" style="position:absolute;height:20px;width:20px;left:3px;bottom:3px;background-color:white;transition:.3s;border-radius:50%;"></span>
                        </div>
                        <span style="font-weight:500;">Auto-Updates</span>
                        <span id="autoUpdatesStatus" style="color:#cbd5e1;font-size:.875rem;">(Loading...)</span>
                    </label>
                </div>
                <div id="updateStatus" style="padding:1rem;border-radius:8px;background:rgba(255,255,255,.05);display:none;"><span id="updateMessage"></span></div>
                <button class="btn btn-primary" onclick="manualUpdate()" id="manualUpdateBtn" style="margin-top:1rem;" disabled>Install Latest Update</button>
            </div>
        </div>
    </div>
    <script>
let typing=false,typingTimeout,pendingUpdate=null,configuredNetworksData=[],draggedElement=null;
function startTyping(){typing=true;clearTimeout(typingTimeout);typingTimeout=setTimeout(()=>typing=false,2000)}
function updateConfig(d){if(d&&!typing){document.getElementById('hysteresis').value=parseFloat(d.hysteresis).toFixed(1);document.getElementById('minRunTime').value=parseFloat(d.minRunTime).toFixed(1);document.getElementById('minStopTime').value=d.minStopTime;document.getElementById('minZoneSwitchTime').value=d.minZoneSwitchTime;document.getElementById('maxRunTime').value=d.maxRunTime;document.getElementById('tempOffset').value=d.tempOffset}else if(!d){fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({hysteresis:parseFloat(document.getElementById('hysteresis').value),minRunTime:parseFloat(document.getElementById('minRunTime').value),minStopTime:parseFloat(document.getElementById('minStopTime').value),minZoneSwitchTime:parseFloat(document.getElementById('minZoneSwitchTime').value),maxRunTime:parseFloat(document.getElementById('maxRunTime').value),tempOffset:parseFloat(document.getElementById('tempOffset').value)})}).catch(e=>console.error('Error:',e))}}
function copyIP(){const ip=document.getElementById('ipAddress').textContent;if(ip&&ip!=='-'){navigator.clipboard.writeText('http://'+ip).then(()=>alert('Copied: http://'+ip)).catch(()=>alert('Copy: http://'+ip))}}
function updateWiFiStatus(){fetch('/api/status').then(r=>r.json()).then(d=>{const s=document.getElementById('currentStatus'),ss=document.getElementById('connectedSSID'),ip=document.getElementById('ipAddress'),ipl=document.getElementById('ipAddressLink');if(d.wifiConnected){s.textContent='Connected';s.style.color='#4ade80';ss.textContent=d.wifiSSID||'N/A';ip.textContent=d.wifiIP||'N/A';ipl.href='http://'+d.wifiIP}else{s.textContent='AP Mode Only';s.style.color='#f59e0b';ss.textContent='-';ip.textContent=d.apIP||'N/A';ipl.href='http://'+d.apIP}}).catch(e=>console.error('WiFi status error:',e))}
function scanNetworks(){const btn=document.getElementById('scanBtn'),txt=document.getElementById('scanBtnText');btn.disabled=true;txt.innerHTML='Scanning...';fetch('/api/wifi/scan').then(r=>r.json()).then(nets=>{const sel=document.getElementById('networkSelect');sel.innerHTML='<option value="">-- Select --</option>';nets.sort((a,b)=>b.quality-a.quality).forEach(n=>{const o=document.createElement('option');o.value=n.ssid;o.textContent=n.ssid+' ('+n.quality+'%)';sel.appendChild(o)});document.getElementById('networkList').style.display='block';btn.disabled=false;txt.textContent='Scan Networks'}).catch(e=>{alert('Scan failed');btn.disabled=false;txt.textContent='Scan Networks'})}
function selectNetwork(){const v=document.getElementById('networkSelect').value;if(v)document.getElementById('wifiSsid').value=v}
function loadConfiguredNetworks(){fetch('/api/wifi/networks').then(r=>r.json()).then(d=>{configuredNetworksData=d.networks||[];renderConfiguredNetworks()}).catch(e=>console.error('Load networks error:',e))}
function renderConfiguredNetworks(){const c=document.getElementById('configuredNetworks');c.innerHTML='';for(let i=0;i<5;i++){const n=configuredNetworksData[i],d=document.createElement('div');if(n&&n.ssid){d.className='network-card';d.innerHTML='<span class="drag-handle">☰</span><div class="network-info"><span class="network-priority">'+(i+1)+'</span><div class="network-details"><div class="network-ssid">'+n.ssid+'</div></div></div><div class="network-actions"><button class="btn btn-small" onclick="removeNetwork('+i+')">Remove</button></div>'}else{d.className='network-card empty-slot';d.innerHTML='<div class="network-info"><span class="network-priority">'+(i+1)+'</span><div class="network-details"><div class="network-ssid" style="color:rgba(255,255,255,.3);">Empty</div></div></div>'}c.appendChild(d)}}
function addNetwork(){const s=document.getElementById('wifiSsid').value.trim(),p=document.getElementById('wifiPassword').value;if(!s){alert('Enter SSID');return}if(configuredNetworksData.some(n=>n&&n.ssid===s)){alert('Already exists');return}let slot=-1;for(let i=0;i<5;i++){if(!configuredNetworksData[i]||!configuredNetworksData[i].ssid){slot=i;break}}if(slot===-1){alert('Max 5 networks');return}configuredNetworksData[slot]={id:slot,ssid:s,password:p,enabled:true};document.getElementById('wifiSsid').value='';document.getElementById('wifiPassword').value='';renderConfiguredNetworks();alert('Added! Click Save to apply.')}
function removeNetwork(i){if(confirm('Remove '+configuredNetworksData[i].ssid+'?')){for(let j=i;j<4;j++)configuredNetworksData[j]=configuredNetworksData[j+1]||{id:j,ssid:'',password:'',enabled:true};configuredNetworksData[4]={id:4,ssid:'',password:'',enabled:true};renderConfiguredNetworks();alert('Removed! Click Save to apply.')}}
function saveAllNetworks(){const nets=[];for(let i=0;i<5;i++){if(configuredNetworksData[i]&&configuredNetworksData[i].ssid)nets.push({ssid:configuredNetworksData[i].ssid,password:configuredNetworksData[i].password==='********'?'':configuredNetworksData[i].password})}if(!nets.length){alert('No networks');return}fetch('/api/wifi/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({networks:nets})}).then(r=>r.json()).then(d=>{if(d.success){alert('Saved!');setTimeout(()=>{loadConfiguredNetworks();updateWiFiStatus()},2000)}else alert('Error: '+d.error)}).catch(e=>alert('Save failed'))}
function loadOTAStatus(){fetch('/api/ota/status').then(r=>r.json()).then(d=>{document.getElementById('currentVersion').textContent=d.currentVersion;document.getElementById('lastChecked').textContent=d.lastChecked>0?new Date(d.lastChecked*1000).toLocaleString():'Never';const t=document.getElementById('autoUpdatesToggle'),sl=document.getElementById('toggleSlider'),kn=document.getElementById('toggleKnob'),st=document.getElementById('autoUpdatesStatus');t.checked=d.autoUpdatesEnabled;if(d.autoUpdatesEnabled){sl.style.backgroundColor='rgba(34,197,94,.5)';kn.style.transform='translateX(24px)';st.textContent='(Enabled)';st.style.color='#4ade80'}else{sl.style.backgroundColor='rgba(239,68,68,.5)';kn.style.transform='translateX(0)';st.textContent='(Disabled)';st.style.color='#f87171'}}).catch(e=>console.error('OTA status error:',e))}
function toggleAutoUpdates(){const t=document.getElementById('autoUpdatesToggle'),sl=document.getElementById('toggleSlider'),kn=document.getElementById('toggleKnob'),st=document.getElementById('autoUpdatesStatus'),en=t.checked;if(en){sl.style.backgroundColor='rgba(34,197,94,.5)';kn.style.transform='translateX(24px)'}else{sl.style.backgroundColor='rgba(239,68,68,.5)';kn.style.transform='translateX(0)'}st.textContent='(Saving...)';fetch('/api/ota/auto-updates',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({enabled:en})}).then(r=>r.json()).then(d=>{if(d.success){st.textContent=d.autoUpdatesEnabled?'(Enabled)':'(Disabled)';st.style.color=d.autoUpdatesEnabled?'#4ade80':'#f87171'}else{t.checked=!en;loadOTAStatus();alert('Failed')}}).catch(e=>{t.checked=!en;loadOTAStatus();alert('Error')})}
function checkForUpdates(){const btn=document.getElementById('checkUpdateBtn'),orig=btn.textContent;btn.disabled=true;btn.textContent='Checking...';fetch('/api/ota/check').then(r=>r.json()).then(d=>{const sd=document.getElementById('updateStatus'),msg=document.getElementById('updateMessage'),ib=document.getElementById('manualUpdateBtn');if(d.updateAvailable){pendingUpdate={version:d.latestVersion,downloadUrl:d.downloadUrl,releaseNotes:d.releaseNotes};let notes=d.releaseNotes||'No notes';if(notes.length>500)notes=notes.substring(0,500)+'...';msg.innerHTML='<strong>Update Available!</strong><br>New: '+d.latestVersion+'<br>Current: '+d.currentVersion+'<hr style="border:none;border-top:1px solid rgba(255,255,255,.2);margin:.5rem 0;"><strong>Notes:</strong><br><div style="white-space:pre-wrap;font-size:.85rem;max-height:200px;overflow-y:auto;">'+notes+'</div>';sd.style.borderLeft='4px solid #22c55e';sd.style.background='rgba(34,197,94,.1)';ib.disabled=false;ib.style.background='rgba(34,197,94,.3)';ib.textContent='Install '+d.latestVersion}else{pendingUpdate=null;msg.innerHTML='Up to date! ('+d.currentVersion+')';sd.style.borderLeft='4px solid #3b82f6';sd.style.background='rgba(59,130,246,.1)';ib.disabled=true;ib.style.background='';ib.textContent='Install Latest Update'}sd.style.display='block';loadOTAStatus()}).catch(e=>{const sd=document.getElementById('updateStatus'),msg=document.getElementById('updateMessage');msg.innerHTML='Error checking updates';sd.style.borderLeft='4px solid #ef4444';sd.style.background='rgba(239,68,68,.1)';sd.style.display='block';setTimeout(()=>sd.style.display='none',5000)}).finally(()=>{btn.disabled=false;btn.textContent=orig})}
function manualUpdate(){if(!pendingUpdate){alert('Click Check for Updates first');return}if(confirm('Install '+pendingUpdate.version+'?')){const btn=document.getElementById('manualUpdateBtn'),orig=btn.textContent;btn.disabled=true;btn.textContent='Installing...';const sd=document.getElementById('updateStatus'),msg=document.getElementById('updateMessage');msg.innerHTML='<strong>Installing...</strong><br>Please wait.';sd.style.borderLeft='4px solid #f59e0b';sd.style.background='rgba(245,158,11,.1)';fetch('/api/ota/update',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({downloadUrl:pendingUpdate.downloadUrl})}).then(r=>r.json()).then(d=>{if(d.success){msg.innerHTML='<strong>'+d.message+'</strong><br>Device restarting...';sd.style.borderLeft='4px solid #22c55e';sd.style.background='rgba(34,197,94,.1)';let c=30;const iv=setInterval(()=>{c--;if(c>0)msg.innerHTML='<strong>Update installed!</strong><br>Refresh in '+c+'s';else{clearInterval(iv);location.reload()}},1000)}else{msg.innerHTML='<strong>Failed:</strong> '+d.message;sd.style.borderLeft='4px solid #ef4444';btn.disabled=false;btn.textContent=orig}}).catch(e=>{msg.innerHTML='<strong>Update may be in progress...</strong><br>Wait and refresh.';setTimeout(()=>location.reload(),30000)})}}
window.addEventListener('load',()=>{document.querySelectorAll('select').forEach(el=>{el.addEventListener('change',startTyping)});fetch('/api/config').then(r=>r.json()).then(d=>updateConfig(d)).catch(e=>console.error('Config error:',e));updateWiFiStatus();loadConfiguredNetworks();loadOTAStatus();setInterval(updateWiFiStatus,10000);setInterval(loadOTAStatus,30000)});
    </script>
</body>
</html>
)rawliteral";

#endif // HTML_SETTINGS_H
