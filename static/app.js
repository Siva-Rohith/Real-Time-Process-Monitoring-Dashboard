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
    // --- UPDATED CHART CONFIG (AUTO-ZOOM ENABLED) ---
    const ctx = document.getElementById('heapChart').getContext('2d');
    const heapChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: Array(20).fill(''),
            datasets: [{
                label: 'Free Heap',
                data: [],
                borderColor: '#58a6ff',
                backgroundColor: 'rgba(88, 166, 255, 0.1)',
                fill: true,
                pointRadius: 4, // Larger points for visibility
                borderWidth: 3,
                tension: 0.4
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
            animation: { duration: 400 }, // Smooth transitions
            plugins: { legend: { display: false } }
        }
    });

    // --- UI RENDERING (WITH RESUME/BLOCK LOGIC) ---
    function updateUI(data) {
        const tableBody = document.getElementById('process-table-body');
        tableBody.innerHTML = '';
        let totalCPU = 0;

        processes.forEach(p => {
            totalCPU += p.cpu;
            const rowClass = p.state === 'KILLED' ? 'text-muted' : (p.cpu > 70 ? 'critical-row' : '');
            
            // Logic to swap buttons based on state
            let actionButtons = '';
            if (p.state === 'KILLED') {
                actionButtons = `<button class="btn btn-sm btn-secondary" disabled>Terminated</button>`;
            } else if (p.state === 'BLOCKED') {
                actionButtons = `
                    <button class="btn btn-sm btn-success me-1" onclick="manageProcess(${p.pid}, 'RESUME')">Resume</button>
                    <button class="btn btn-sm btn-danger" onclick="manageProcess(${p.pid}, 'KILL')">Kill</button>`;
            } else {
                actionButtons = `
                    <button class="btn btn-sm btn-warning me-1" onclick="manageProcess(${p.pid}, 'BLOCK')">Suspend</button>
                    <button class="btn btn-sm btn-danger" onclick="manageProcess(${p.pid}, 'KILL')">Kill</button>`;
            }

            const row = `<tr class="${rowClass}">
                <td>${p.pid}</td>
                <td><strong>${p.name}</strong></td>
                <td><span class="badge ${getStatusClass(p.state)}">${p.state}</span></td>
                <td>
                    <div class="d-flex align-items-center">
                        <span class="me-2">${p.cpu}%</span>
                        <div class="progress w-100"><div class="progress-bar ${p.cpu > 60 ? 'bg-danger' : 'bg-info'}" style="width: ${p.cpu}%"></div></div>
                    </div>
                </td>
                <td>${p.mem.toLocaleString()} B</td>
                <td class="text-center">${actionButtons}</td>
            </tr>`;
            tableBody.insertAdjacentHTML('beforeend', row);
        });

        // Update Top Cards
        document.getElementById('total-heap').innerText = data.heap.toLocaleString() + " B";
        document.getElementById('heap-bar').style.width = (data.heap / MAX_HEAP * 100) + "%";
        document.getElementById('cpu-load-text').innerText = totalCPU + "%";
        document.getElementById('cpu-bar').style.width = totalCPU + "%";
        document.getElementById('uptime').innerText = new Date(data.uptime * 1000).toISOString().substr(11, 8);
    }

    function getStatusClass(state) {
        if (state === 'RUNNING') return 'status-running';
        if (state === 'READY') return 'status-ready';
        if (state === 'BLOCKED') return 'status-blocked';
        return 'status-killed';
    }





       

    

    