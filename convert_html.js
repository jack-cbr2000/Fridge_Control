const fs = require('fs');
const path = require('path');

// HTML files to convert
const files = [
  { name: 'index.html', variable: 'HTML_INDEX' },
  { name: 'basic.html', variable: 'HTML_BASIC' },
  { name: 'manual.html', variable: 'HTML_MANUAL' },
  { name: 'charts.html', variable: 'HTML_CHARTS' },
  { name: 'settings.html', variable: 'HTML_SETTINGS' }
];

console.log('Converting HTML files to PROGMEM arrays...\n');

// Process each file
files.forEach(file => {
  const filePath = path.join('data', file.name);
  
  if (!fs.existsSync(filePath)) {
    console.error(`Error: ${filePath} not found!`);
    return;
  }

  const html = fs.readFileSync(filePath, 'utf8');
  const lines = html.split('\n');
  
  // Build the header file content
  let headerContent = `// Auto-generated from ${file.name}\n`;
  headerContent += `// Do not edit manually - regenerate with: node convert_html.js\n\n`;
  headerContent += `#ifndef ${file.variable}_H\n`;
  headerContent += `#define ${file.variable}_H\n\n`;
  headerContent += `#include <Arduino.h>\n\n`;
  headerContent += `const char ${file.variable}[] PROGMEM = R"====(\n`;
  headerContent += html;
  headerContent += `\n)====" ;\n\n`;
  headerContent += `#endif // ${file.variable}_H\n`;
  
  // Write to include directory
  const headerPath = path.join('include', `${file.variable.toLowerCase()}.h`);
  fs.writeFileSync(headerPath, headerContent);
  
  console.log(`✓ Generated ${headerPath} (${html.length} bytes)`);
});

console.log('\n✅ All HTML files converted successfully!');
console.log('\nNext steps:');
console.log('1. Include the headers in main.cpp');
console.log('2. Modify page serving functions to use PROGMEM arrays');
console.log('3. Upload firmware via OTA - web files will update automatically!\n');
