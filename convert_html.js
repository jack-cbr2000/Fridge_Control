const fs = require('fs');
const html = fs.readFileSync('data/index.html', 'utf8');
const lines = html.split('\n');

let output = 'String html = ';
for (let i = 0; i < lines.length; i++) {
  const escaped = lines[i].replace(/\\/g, '\\\\').replace(/"/g, '\\"').replace(/'/g, "\\'");
  if (i === 0) {
    output += '"' + escaped + '\\n";\n';
  } else {
    output += 'html += "' + escaped + '\\n";\n';
  }
}
console.log(output);
