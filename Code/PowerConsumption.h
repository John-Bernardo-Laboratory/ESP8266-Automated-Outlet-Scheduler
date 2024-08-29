const char PowerCons[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Power Consumption</title>
    <link rel="icon" type="image/png" href="https://i.ibb.co/kgxMQmx/green-Energy.png">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://code.highcharts.com/highcharts.js"></script>
    <style>
        body {
            font-family: sans-serif;
            background-color: #1e1e1e;
            color: #ffffff;
        }

        .mainContainer {
            display: flex;
        }

        .container {
            padding: 10px;
            display: flex;
            justify-content: center;
            width: 80vw;
            flex-wrap: wrap;
            height: 45vh;
        }

        .chart-container {
            height: 100%;
            width: auto;
            padding-left: 10px;
            display: flex;
        }

        table {
            border-collapse: collapse;
            font-size: small;
            border-radius: 1vw;
            overflow: hidden;
            border: 1vh solid #d4d4d4;
            height: 100%;
            color: #d4d4d4;
            width: 35vw;
        }

        th,
        td {
            border: 1px solid #313131;
            padding: 5px;
            text-align: left;
        }

        th {
            background-color: #333333;
        }

        #chart {
            height: auto;

        }

        .limitBox {
            font-family: sans-serif;
            border: 1px solid #ccc;
            padding: 10px;
            margin-bottom: 10px;
            max-height: 84vh;
            max-width: 200px;
            border: none;
            overflow-y: auto;
            flex: 1;
        }

        .item {
            align-content: center;
            padding-left: 10px;
            cursor: pointer;
            margin-bottom: 5px;
            position: relative;
            background-color: #ad0855;
            border-radius: 5px;
            max-width: 200px;
            height: 30px;
            color: white;
            transition: transform 0.5s ease;
        }

        .item:hover {
            transform: scale(1.05);
        }

        .item input {
            border: none;
            outline: none;
        }

        .remove-button {
            position: absolute;
            right: 0;
            top: 50%;
            transform: translateY(-50%);
            background-color: transparent;
            border: none;
            cursor: pointer;
            border-radius: 50%;
            font-size: 12px;
            color: white;
        }

        .add-button {
            margin: 5px;
            padding: 5px 10px;
            background-color: #ad0855;
            width: 16vw;
            border: none;
            color: white;
            height: 6vh;
            letter-spacing: .05vw;
            border-radius: 2.3vh;
            cursor: pointer;
        }

        .add-button:hover {
            background-color: #82043f;
        }


        .limitBox::-webkit-scrollbar {
            width: 10px;
        }

        .limitBox::-webkit-scrollbar-track {
            background-color: transparent;
        }

        .limitBox::-webkit-scrollbar-thumb {
            background-color: #ad0855;
            border-radius: 5px;
        }

        .limitBox::-webkit-scrollbar-thumb:hover {
            background-color: #82043f;
        }

        .popup {
            display: none;
            position: fixed;
            top: 50%;
            font-size: 1.5vw;
            left: 50%;
            transform: translate(-50%, -50%);
            background-color: rgba(25, 25, 26, 0.7);
            padding: 5vh;
            width: 32vw;
            height: 5vh;
            align-items: center;
            align-content: center;
            border-radius: 1vw;
            box-shadow: 0 0 2vh rgba(0, 0, 0, 0.5);
            z-index: 4;
        }

        .popup button {
            margin-left: 1vw;
            padding: 0.6vw 2vw;
            font-size: 1.3vw;
            background-color: #ad0855;
            color: white;
            border: none;
            border-radius: 2.3vh;
            cursor: pointer;
        }

        .popup input[type="time"] {
            width: 8vw;
            height: 1.5vw;
            font-size: 1.2vw;
            padding: 1vh;
            border: none;
            border-radius: 0.9vw;
        }

        .popup button:hover {
            background-color: #82043f;
        }

        .close-btn {
            position: absolute;
            top: 1vw;
            right: 1vw;
            font-size: 1vw;
            cursor: pointer;
            transition: transform 0.3s ease;
            width: 1.5vw;
            height: 1.5vw;
            line-height: 3vh;
            text-align: center;
        }

        .close-btn:hover::before {
            content: '';
            position: absolute;
            top: -1vh;
            right: -1vh;
            bottom: -1vh;
            left: -1vh;
            background-color: red;
            opacity: 75%;
            z-index: -1;
            border-radius: 20%;
        }

        #overlay {
            display: none;
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: rgba(0, 0, 0, 0.5);
            z-index: 3;
        }
    </style>

    <script>
        let items = [];
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                items = JSON.parse(this.responseText);
                renderItems();
            }
        };
        xhttp.open("GET", "/getEnergyLimit", true);
        xhttp.send();
    </script>

</head>

<body>

    <div class="mainContainer">
        <div>
            <button class="add-button" onclick="addItem()">Add Energy Limit (kWh)</button>
            <div class="limitBox" id="limitBox"></div>
        </div>

        <div>
            <div class="container">
                <table>
                    <tr>
                        <th>Parameter</th>
                        <th>Value</th>
                    </tr>
                    <tr>
                        <td>Voltage</td>
                        <td id="voltage">--</td>
                    </tr>
                    <tr>
                        <td>Current</td>
                        <td id="current">--</td>
                    </tr>
                    <tr>
                        <td>Frequency</td>
                        <td id="frequency">--</td>
                    </tr>
                    <tr>
                        <td>Active Power</td>
                        <td id="activePower">--</td>
                    </tr>
                    <tr>
                        <td>Power Factor</td>
                        <td id="powerFactor">--</td>
                    </tr>
                    <tr>
                        <td>Apparent Power</td>
                        <td id="apparentPower">--</td>
                    </tr>
                    <tr>
                        <td>Reactive Power</td>
                        <td id="reactivePower">--</td>
                    </tr>
                </table>
                <div class="chart-container">
                    <canvas id="barChart"></canvas>
                </div>
            </div>

            <div class="container">
                <div id="chart" class="container"></div>
            </div>
        </div>
    </div>

    <div id="overlay"></div>
    <div class="popup" id="popup">
        <span class="close-btn" onclick="closePopup()">x</span>
        When <input type="date" id="date">
        Limit <input type="text" id="energy" size="5">
        <button onclick="addItemInput()">Add</button>
    </div>

    <script>
        let barData = {
            labels: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'],
            datasets: [{
                data: [],
                backgroundColor: [
                    'rgba(255, 99, 132, 0.2)',
                    'rgba(54, 162, 235, 0.2)',
                    'rgba(255, 206, 86, 0.2)',
                    'rgba(75, 192, 192, 0.2)',
                    'rgba(153, 102, 255, 0.2)',
                    'rgba(255, 159, 64, 0.2)',
                    'rgba(255, 99, 132, 0.2)',
                    'rgba(54, 162, 235, 0.2)',
                    'rgba(255, 206, 86, 0.2)',
                    'rgba(75, 192, 192, 0.2)',
                    'rgba(153, 102, 255, 0.2)',
                    'rgba(255, 159, 64, 0.2)'
                ],
                borderColor: [
                    'rgba(255, 99, 132, 1)',
                    'rgba(54, 162, 235, 1)',
                    'rgba(255, 206, 86, 1)',
                    'rgba(75, 192, 192, 1)',
                    'rgba(153, 102, 255, 1)',
                    'rgba(255, 159, 64, 1)',
                    'rgba(255, 99, 132, 1)',
                    'rgba(54, 162, 235, 1)',
                    'rgba(255, 206, 86, 1)',
                    'rgba(75, 192, 192, 1)',
                    'rgba(153, 102, 255, 1)',
                    'rgba(255, 159, 64, 1)'
                ],
                borderWidth: 1
            }],
        };

        let barOptions = {
            plugins: {
                legend: {
                    display: false
                },
                title: {
                    display: true,
                    text: 'Energy',
                    color: '#d4d4d4',
                    font: {
                        size: 16,
                        weight: 'bold',
                        family: 'sans-serif',
                        lineHeight: 0.5,
                    }
                }, tooltip: {
                    callbacks: {
                        label: function (context) {
                            var label = context.dataset.label || '';
                            if (label) {
                                label += ': ';
                            }
                            if (context.parsed.y !== null) {
                                label += context.parsed.y.toFixed(2) + ' kWh';
                            }
                            return label;
                        }
                    }
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    max: 0,
                    ticks: {
                        color: '#d4d4d4',
                    },
                    grid: {
                        color: 'rgba(255, 255, 255, 0.02)',
                        zIndex: -1,
                    },
                    title: {
                        display: true,
                        text: 'kilowatt hour (kWh)',
                        color: '#d4d4d4'
                    }
                },
                x: {
                    grid: {
                        display: false
                    },
                    ticks: {
                        color: '#d4d4d4',
                    }
                }
            }
        };

        let barCtx = document.getElementById('barChart').getContext('2d');
        let barChart = new Chart(barCtx, {
            type: 'bar',
            data: barData,
            options: barOptions
        });

        var chartTContainer = document.getElementById('chart');
        chartTContainer.style.width = '100%';

        var chartT = new Highcharts.Chart({
            chart: {
                renderTo: 'chart',
                backgroundColor: 'transparent'
            },
            title: {
                text: 'Active Power',
                style: {
                    color: '#d4d4d4',
                    fontSize: '16px'
                }
            },
            series: [{
                showInLegend: false,
                data: []
            }],
            plotOptions: {
                line: {
                    animation: false,
                    dataLabels:
                    {
                        enabled: true,
                        color: '#d4d4d4'
                    }
                },
                series: { color: '#ad0855' }
            },
            tooltip: {
                formatter: function () {
                    return '<b>' + this.y + ' W</b>';
                }
            },
            xAxis: {
                type: 'datetime',
                dateTimeLabelFormats: { second: '%H:%M:%S' },
                labels: {
                    style: {
                        color: '#d4d4d4'
                    }
                }
            },
            yAxis: {
                title: {
                    text: 'Watt (W)',
                    style: {
                        color: '#d4d4d4'
                    }
                },
                labels: {
                    enabled: false
                },
                gridLineWidth: 0,
            },
            credits: { enabled: false },
            accessibility: {
                enabled: false
            }
        });
        setInterval(function () {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    var response = JSON.parse(this.responseText);
                    var watt = parseFloat(response.activePower);
                    var x = (new Date()).getTime() + 28800000;
                    document.getElementById("voltage").textContent = response.voltage.toFixed(2) + " V";
                    document.getElementById("current").textContent = response.current.toFixed(2) + " A";
                    document.getElementById("activePower").textContent = response.activePower.toFixed(2) + " W";
                    document.getElementById("frequency").textContent = response.frequency.toFixed(2) + " Hz";
                    document.getElementById("powerFactor").textContent = response.powerFactor.toFixed(2) + " PF";
                    document.getElementById("apparentPower").textContent = response.apparentPower.toFixed(2) + " VA";
                    document.getElementById("reactivePower").textContent = response.reactivePower.toFixed(2) + " VAR";

                    var energy = response.energy;
                    if (Array.isArray(energy) && energy.length > 0) {
                        barData.datasets[0].data = energy;
                        let maxDataValue = Math.max(...energy);
                        barOptions.scales.y.max = maxDataValue;
                        barChart.options = barOptions;
                        barChart.update();
                    }

                    if (chartT.series[0].data.length > 40) {
                        chartT.series[0].addPoint([x, watt], true, true, true);
                    } else {
                        chartT.series[0].addPoint([x, watt], true, false, true);
                    }
                }
            };
            xhttp.open("GET", "/powerData", true);
            xhttp.send();
        }, 1000);

        const monthsName = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];

        function renderItems() {
            var limitBox = document.getElementById('limitBox');
            limitBox.innerHTML = '';
            items.forEach(function (item, index) {
                var itemDiv = document.createElement('div');
                itemDiv.className = 'item';
                itemDiv.dataset.index = index;

                var textSpan = document.createElement('span');
                textSpan.innerText = `${monthsName[item[0]]} ${item[1]}: ${item[2]} kWh`;

                var removeButton = document.createElement('button');
                removeButton.className = 'remove-button';
                removeButton.innerHTML = 'x';
                removeButton.onclick = function (event) {
                    event.stopPropagation();
                    removeItem(index);
                };

                itemDiv.appendChild(textSpan);
                itemDiv.appendChild(removeButton);
                limitBox.appendChild(itemDiv);
            });
        }

        function addItemInput() {
            let date = document.getElementById('date').value;
            let energyLimit = document.getElementById('energy').value;
            let selectedDate = new Date(date);
            let month = selectedDate.getMonth();
            let day = selectedDate.getDate();

            if ((parseInt(month) >= 0) && day && energyLimit) {
                items.push([parseInt(month), parseInt(day), parseFloat(energyLimit.trim())]);
                renderItems();
            } else {
                alert("Please enter valid inputs.");
                return;
            }

            closePopup();
            updateEnergyLimit();
            renderItems();
        }

        function addItem() {
            document.getElementById('overlay').style.display = 'block';
            document.getElementById('popup').style.display = 'block';
        }

        function closePopup() {
            document.getElementById('popup').style.display = 'none';
            document.getElementById('overlay').style.display = 'none';
        }
        document.body.addEventListener('dblclick', (event) => {
            let addPopup = document.getElementById('popup');
            if (addPopup.style.display === 'block') {
                if (!addPopup.contains(event.target)) {
                    closePopup();
                }
            }
        });

        function removeItem(index) {
            items.splice(index, 1);
            updateEnergyLimit()
            renderItems();
        }

        function updateEnergyLimit() {
            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/updateEnergyLimit", true);
            var params = "energyLimit=" + encodeURIComponent(JSON.stringify(items));
            xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            xhr.onreadystatechange = function () {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    console.log(xhr.responseText);
                }
            }
            xhr.send(params);
        }
        renderItems();
    </script>
</body>

</html>
)=====";