const char WifiSetup[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang='en'>

<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>Wifi Setup</title>
    <link rel="icon" type="image/png" href="https://i.ibb.co/kgxMQmx/green-Energy.png">
    <style>
        body {
            margin: 0;
            font-family: sans-serif;
            font-size: 1rem;
            font-weight: 400;
            line-height: 1.5;
            color: #ffffff;
            background-color: #1e1e1e;
        }

        .control {
            display: block;
            width: 100%;
            height: calc(1.5em + .75rem + 2px);
            border: 1px solid #ced4da;
        }

        button {
            cursor: pointer;
            border: 1px solid transparent;
            color: #fff;
            background-color: #ad0855;
            padding: .5rem;
            font-size: 1.25rem;
            line-height: 1.5;
            border-radius: .3rem;
            width: 100%;
            transition: transform 0.3s ease;
            margin-left: 0.15rem;
        }

        button:hover {
            background-color: #82043f;
        }

        button:active {
            transform: scale(1.05);
        }

        .signin {
            width: 100%;
            max-width: 200px;
            padding: 15px;
            margin: auto;
        }

        h1 {
            text-align: center;
            color: #d4d4d4;
        }

        .error-message {
            color: red;
            font-size: 0.8rem;
            text-align: center;
            margin-top: 5px;
        }

        .loading {
            display: none;
            border: 8px solid #f3f3f3;
            border-top: 8px solid #ad0855;
            border-radius: 50%;
            width: 50px;
            height: 50px;
            animation: spin 1s linear infinite;
            margin: auto;
            margin-top: 50px;
        }

        .doneSetup {
            display: none;
            text-align: center;
        }

        @keyframes spin {
            0% {
                transform: rotate(0deg);
            }

            100% {
                transform: rotate(360deg);
            }
        }
    </style>

    <script>
        let ipAddr = "";
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                ipAddr = this.responseText;
                console.log(ipAddr);
            }
        };
        xhttp.open("GET", "/ipAddr", true);
        xhttp.send();

        function updateWifi() {
            var ssid = document.getElementById('ssid').value;
            var password = document.getElementById('password').value;

            if (ssid === '') {
                document.getElementById('error-message-ssid').innerText = 'SSID is required';
                return;
            } else {
                document.getElementById('error-message-ssid').innerText = '';
            }

            if (password === '') {
                document.getElementById('error-message-password').innerText = 'Password is required';
                return;
            } else {
                document.getElementById('error-message-password').innerText = '';
            }

            var data = "ssid=" + encodeURIComponent(ssid) + "&password=" + encodeURIComponent(password);

            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/updateWifi", true);
            xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            xhr.onreadystatechange = function () {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    console.log(xhr.responseText);
                }
            }
            xhr.send(data);

            document.getElementById('saveButton').style.display = 'none';

            if (ipAddr == '192.168.4.1') {
                document.getElementById('doneSetup').style.display = 'block';
            } else {
                document.getElementById('loading').style.display = 'block';
            }
        }

        setInterval(function () {
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    var response = this.responseText;
                    if (response == 'loaded' && document.getElementById('loading').style.display == 'block' && ipAddr != '192.168.4.1') {
                        window.location.href = "http://" + ipAddr;
                    }
                }
            };
            xhttp.open("GET", "/response", true);
            xhttp.send();
        }, 500);
    </script>
</head>

<body>
    <div class='signin'>
        <h1>Wifi Setup</h1><br />
        <div><label>SSID</label><input type='text' class='control' id='ssid'> </div>
        <div id="error-message-ssid" class="error-message"></div>
        <div><br /><label>Password</label><input type='password' class='control' id='password'></div>
        <div id="error-message-password" class="error-message"></div>
        <br /><br /><button id="saveButton" type='button' onclick='updateWifi()'>Save</button>
        <div id="loading" class="loading"></div>
        <div id="doneSetup" class="doneSetup">Done Setup</div>
    </div>
</body>

</html>
)=====";