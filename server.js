const http = require('http');

const hostname = '0.0.0.0'; // Listen on all network interfaces
const port = 3000;

const server = http.createServer((req, res) => {
  if (req.method === 'POST' && req.url === '/') {
    let body = '';

    // Collect the incoming data
    req.on('data', chunk => {
      body += chunk.toString();
    });

    req.on('end', () => {
      try {
        const data = JSON.parse(body); // Parse the JSON payload
        console.log('Received data:', data);

        // Respond with success
        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ message: 'Data received successfully' }));
      } catch (error) {
        console.error('Invalid JSON format:', error.message);

        // Respond with error
        res.writeHead(400, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ error: 'Invalid JSON format' }));
      }
    });
  } else {
    // Respond with 404 for unrecognized routes
    res.writeHead(404, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ error: '404 Not Found' }));
  }
});

server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});
