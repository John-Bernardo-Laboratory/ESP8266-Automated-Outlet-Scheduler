const char htmlMain[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Main</title>
    <link rel="icon" type="image/png" href="https://i.ibb.co/kgxMQmx/green-Energy.png">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&family=Montserrat:wght@400;700&display=swap');
        @import url('https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css');

        body {
            font-family: sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            margin: 0;
            background-color: #1e1e1e;
        }

        .cover-box {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            width: 60vw;
            height: 35vh;
            background-color: #252526;
            border-radius: 2vw;
            box-shadow: 0 0 2vw rgba(0, 0, 0, 0.2);
            display: flex;
            justify-content: center;
            align-items: center;
            z-index: 1;
        }

        .container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        label {
            position: absolute;
            top: 20vh;
            left: 0;
            font-size: 3vw;
            font-weight: bold;
            color: #d4d4d4;
            margin: 0;
            z-index: 1;
            animation: slideFromLeft 1s forwards;
            letter-spacing: 1.5vw;
            white-space: nowrap;
        }

        @keyframes slideFromLeft {
            0% {
                left: -60vw;
            }

            100% {
                transform: translateX(-50%);
                left: 50%;
            }
        }

        .box {
            width: 12vw;
            height: 25vh;
            margin: 1vw;
            text-align: center;
            align-content: center;
            font-size: 1.5vw;
            border-radius: 2vw;
            cursor: pointer;
            box-shadow: 0 0 1vw rgba(0, 0, 0, 0.1);
            transition: transform 0.5s ease;
            border: 0.5vw solid #fff;
            background-color: #f0a6ca;
            background-image: linear-gradient(to bottom right, #f0a6ca, #f6d365);
            color: #fff;
            z-index: 1;
        }

        .box:hover {
            color: #383838;
            box-shadow: 0 0 3vw rgba(0, 0, 0, 0.2);
        }

        .outlet-2 {
            background-image: linear-gradient(to bottom right, #b5ffd9, #9198e5);
            color: #fff;
        }

        .outlet-3 {
            background-image: linear-gradient(to bottom right, #9966ff, #ff7b83);
            color: #fff;
        }

        .outlet-4 {
            background-image: linear-gradient(to bottom right, #66ffdb, #d0e60f);
            color: #fff;
        }

        .power-button {
            position: absolute;
            right: center;
            bottom: 20vh;
            padding: 1vw 2vw;
            font-size: 1.3vw;
            background-color: #2b2b2b;
            color: #b3b3b3;
            border: none;
            border-radius: 2vw;
            cursor: pointer;
            letter-spacing: 1vw;
            transition: background-color 0.5s ease-in-out, transform 0.3s ease;
            z-index: 1;
        }

        .power-button:hover {
            background-color: #383838;
        }

        .scale-down {
            transform: scale(0.9);
        }

        .wifiBtn {
            position: absolute;
            border: none;
            color: #b3b3b3;
            background-color: transparent;
            font-size: 1vw;
            line-height: 1.5;
            border-radius: 2vw;
            width: 15vw;
            letter-spacing: 1vh;
            white-space: nowrap;
            transition: color 0.3s ease-in-out, transform 0.3s ease;
            bottom: 2vh;
        }

        .wifiBtn:hover {
            color: rgb(224, 224, 224);
        }

        .wifiBtn:active {
            transform: scale(1.05);
        }

    </style>
</head>

<body>
    <label>Select Your Outlet</label>
    <div class="cover-box">
        <div class="container">
            <div class="box outlet-1" onclick="updateOutlet(0)">OUTLET 1</div>
            <div class="box outlet-2" onclick="updateOutlet(1)">OUTLET 2</div>
            <div class="box outlet-3" onclick="updateOutlet(2)">OUTLET 3</div>
            <div class="box outlet-4" onclick="updateOutlet(3)">OUTLET 4</div>
        </div>
    </div>

    <button class="power-button" onclick="togglePowerButton()">
        Power Consumption <i class="fa fa-bar-chart"></i>
    </button>

    <button class="wifiBtn" onclick="window.location.href = '/Wifi'">
        Change WiFi
    </button>

    <script>
        function togglePowerButton() {
            var powerButton = document.querySelector('.power-button');
            powerButton.classList.add('scale-down');
            setTimeout(function () {
                powerButton.classList.remove('scale-down');
            }, 100);

            window.location.href = "/PowerConsumption";
        }

        function updateOutlet(outletNumber) {
            var boxes = document.querySelectorAll('.box');
            boxes.forEach(function (box, index) {
                if (index === outletNumber) {
                    box.classList.add('scale-down');
                    setTimeout(function () {
                        box.classList.remove('scale-down');
                    }, 100);
                }
            });

            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/outletNumber", true);
            xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            xhr.send("outlet=" + outletNumber);
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    window.location.href = "/Editor?outlet=" + outletNumber;
                }
            };
        }

        window.onload = function () {
            console.log("Loaded");
        }

    </script>

</body>

</html>
)=====";
