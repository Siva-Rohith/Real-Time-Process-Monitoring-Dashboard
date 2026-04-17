    const ESP_IP = "10.223.211.239"; 
    const MAX_HEAP = 52000; 
    let processes = [];

    // --- WEBSOCKET CONNECTION ---
    const socket = new WebSocket(ws://${ESP_IP}:81/);

    socket.onopen = () => {
        document.getElementById('connection-status').className = "badge bg-success p-2";
        document.getElementById('connection-status').innerText = "LIVE";
        logEvent("[SYSTEM] Connection established with NodeMCU.");
    };

    socket.onclose = () => {
        document.getElementById('connection-status').className = "badge bg-danger p-2";
        document.getElementById('connection-status').innerText = "OFFLINE";
        logEvent("[ERROR] Hardware Disconnected.");
    };

    socket.onmessage = (event) => {
        const data = JSON.parse(event.data);
        processes = data.processes;
        updateUI(data);
        
        // Update Chart Data
        heapChart.data.datasets[0].data.push(data.heap);
        if (heapChart.data.datasets[0].data.length > 20) heapChart.data.datasets[0].data.shift();
        heapChart.update();
    };
 // --- CHART INITIALIZATION ---
    
    const ctx = document.getElementById('heapChart').getContext('2d');
    const heapChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: Array(20).fill(''),
            datasets: [{
                data: [],
                backgroundColor: 'rgba(88, 166, 255, 0.1)',
                fill: true
            }]
        },
        options: { 
            responsive: true, 
            maintainAspectRatio: false,
            scales: { 
                x: { display: false }, 
                y: { 
                    grid: { color: '#30363d' },
                    ticks: { color: '#8b949e' },
                    beginAtZero: false // THIS IS THE FIX: Zoom in on the data!
                } 
            },
            plugins: { legend: { display: false } }
        }
    });
