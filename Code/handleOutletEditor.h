const char htmlEditor[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Editor</title>
    <link rel="icon" type="image/png" href="https://i.ibb.co/kgxMQmx/green-Energy.png">
    <style>
        body {
            font-family: sans-serif;
            background-color: #1e1e1e;
            color: #d4d4d4;
        }

        .container {
            background-color: #252526;
            box-shadow: 0 0 2vw rgba(0, 0, 0, 0.2);
            padding: 1% 1%;
            font-size: 1.5vw;
            border-radius: 2vw;
            margin: 6% auto 2%;
            width: 80%;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            table-layout: fixed;
        }

        th,
        td {
            padding: 2%;
            text-align: center;
            width: calc(100% / 7);
            cursor: pointer;
            position: relative;
            line-height: 1.5;
            color: #ffffff;
        }

        th {
            color: #d4d4d4;
            transition: transform 0.3s ease;
        }

        th:hover {
            transform: scale(1.1);
        }

        th:not(:hover) {
            transform: scale(1);
        }

        .time_row {
            height: 65vh;
        }

        .time-range {
            background-color: #ad0855;
            color: white;
            display: flex;
            align-items: center;
            justify-content: center;
            border-radius: 0.7vw;
            font-size: 1.1vw;
            white-space: nowrap;
            position: absolute;
            left: 2%;
            right: 2%;
            padding: 0.5%;
            text-align: center;
            cursor: pointer;
            transition: transform 0.3s ease;
        }

        .time-range:hover {
            transform: scale(1.03);
        }

        .remove-button {
            position: absolute;
            top: -0.4vw;
            right: 0.1vw;
            padding: 0.3vw;
            color: #d4d4d4;
            border-radius: 20%;
            font-size: 1vw;
            cursor: pointer;
        }

        label {
            position: absolute;
            top: 2%;
            left: 50%;
            transform: translateX(-50%);
            font-size: 3vw;
            font-weight: bold;
            color: #d4d4d4;
            margin: 0;
            z-index: 1;
            animation: slideFromLeft 1s forwards;
            letter-spacing: 1vw;
            white-space: nowrap;
        }

        .time-slot {
            color: #d4d4d4;
        }

        @keyframes slideFromLeft {
            0% {
                left: -50%;
            }

            100% {
                left: 50%;
            }
        }

        @keyframes removeTimeRangeAnimation {
            0% {
                opacity: 1;
                scale: 1;
            }

            100% {
                opacity: 0;
                scale: 0.8;
            }
        }

        .remove-animation {
            animation: removeTimeRangeAnimation 0.5s ease forwards;
        }

        @keyframes addTimeRangeAnimation {
            0% {
                opacity: 0;
                transform: scale(0.8);
            }

            100% {
                opacity: 1;
                transform: scale(1);
            }
        }

        .add-animation {
            animation: addTimeRangeAnimation 0.5s ease forwards;
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

        .popup-content {
            color: white;
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
        let timeRanges = { "sunday": [] };
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                timeRanges = JSON.parse(this.responseText);
                updateDisplay();
            }
        };
        xhttp.open("GET", "/getTimeRanges", true);
        xhttp.send();
    </script>
</head>

<body>
    <label>Time Range Editor</label>
    <div class="container">
        <table>
            <thead>
                <tr>
                    <th onclick="addTimeRange('sunday')">Sunday</th>
                    <th onclick="addTimeRange('monday')">Monday</th>
                    <th onclick="addTimeRange('tuesday')">Tuesday</th>
                    <th onclick="addTimeRange('wednesday')">Wednesday</th>
                    <th onclick="addTimeRange('thursday')">Thursday</th>
                    <th onclick="addTimeRange('friday')">Friday</th>
                    <th onclick="addTimeRange('saturday')">Saturday</th>
                </tr>
            </thead>
            <tbody>
                <tr class="time_row">
                    <td class="time-slot"></td>
                    <td class="time-slot"></td>
                    <td class="time-slot"></td>
                    <td class="time-slot"></td>
                    <td class="time-slot"></td>
                    <td class="time-slot"></td>
                    <td class="time-slot"></td>
                </tr>
            </tbody>
        </table>
    </div>
    <div id="overlay"></div>
    <div class="popup" id="popup">
        <div class="popup-content">
            <span class="close-btn" onclick="closePopup()">x</span>
            from <input type="time" id="fromTime" name="fromTime" value="00:00" step="60">
            to <input type="time" id="toTime" name="toTime" value="00:00" step="60">
            <button onclick="addTimeRangeFromPopup()">Add</button>
        </div>
    </div>
    <div class="popup" id="edit-popup">
        <span class="close-btn" onclick="closePopup()">x</span>
        from <input type="time" id="edit-fromTime" name="edit-fromTime" value="00:00" step="60">
        to <input type="time" id="edit-toTime" name="edit-toTime" value="00:00" step="60">
        <button onclick="EditTimeRangeFromPopup()">Edit</button>
    </div>
    <script>
        let selectedDay = null;
        let selectedTimeRange = null;
        document.querySelectorAll('.time-slot').forEach((item, index) => {
            let day = index % 7;
            let timeRangesForDay = timeRanges[getDayName(day)];
            let innerHTML = '';
            if (timeRangesForDay) {
                for (let range of timeRangesForDay) {
                    let [start, end] = range.split(' - ');
                    let [startHour, startMinute] = start.split(':').map(Number);
                    let [endHour, endMinute] = end.split(':').map(Number);
                    let totalStartMinutes = startHour * 60 + startMinute;
                    let totalEndMinutes = endHour * 60 + endMinute;

                    if (totalEndMinutes - totalStartMinutes < 60) {
                        totalEndMinutes = totalStartMinutes + 60;
                        endHour = Math.floor(totalEndMinutes / 60);
                        endMinute = totalEndMinutes % 60;
                        end = `${String(endHour).padStart(2, '0')}:${String(endMinute).padStart(2, '0')}`;
                    }
                    let totalMinutesInDay = 24 * 60;
                    let topOffset = (totalStartMinutes / totalMinutesInDay) * 100;
                    let height = ((totalEndMinutes - totalStartMinutes) / totalMinutesInDay) * 100;
                    innerHTML += `<div class="time-range" style="top: ${topOffset}%; height: ${height}%" onclick="selectTimeRange(this, '${getDayName(day)}', '${range}')">
                                        ${range}
                                        <div class="remove-button" onclick="removeTimeRange(this.parentNode, '${getDayName(day)}', '${range}')">x</div>
                                      </div>`;

                }
            }
            item.innerHTML = innerHTML;
        });
        function getDayName(dayIndex) {
            const days = ['sunday', 'monday', 'tuesday', 'wednesday', 'thursday', 'friday', 'saturday'];
            return days[dayIndex];
        }
        function addTimeRange(day) {
            selectedDay = day;
            document.getElementById('overlay').style.display = 'block';
            document.getElementById('popup').style.display = 'block';
        }
        function addTimeRangeFromPopup() {
            let fromTime = document.getElementById('fromTime').value;
            let toTime = document.getElementById('toTime').value;

            let [startHour, startMinute] = fromTime.split(':').map(Number);
            let [endHour, endMinute] = toTime.split(':').map(Number);
            let totalStartMinutes = startHour * 60 + startMinute;
            let totalEndMinutes = endHour * 60 + endMinute;

            if (totalStartMinutes >= totalEndMinutes) {
                alert("Start time cannot be greater than or equal to end time.");
                return;
            }

            let newTimeRange = `${fromTime} - ${toTime}`;
            if (timeRanges[selectedDay].includes(newTimeRange)) {
                alert("Duplicate time range detected.");
                return;
            }

            timeRanges[selectedDay].push(newTimeRange);
            updateDisplay();
            document.getElementById('popup').style.display = 'none';
            document.getElementById('overlay').style.display = 'none';
        }
        function EditTimeRangeFromPopup() {
            let fromTime = document.getElementById('edit-fromTime').value;
            let toTime = document.getElementById('edit-toTime').value;

            let [startHour, startMinute] = fromTime.split(':').map(Number);
            let [endHour, endMinute] = toTime.split(':').map(Number);
            let totalStartMinutes = startHour * 60 + startMinute;
            let totalEndMinutes = endHour * 60 + endMinute;

            if (totalStartMinutes >= totalEndMinutes) {
                alert("Start time cannot be greater than or equal to end time.");
                return;
            }

            let newTimeRange = `${fromTime} - ${toTime}`;
            let index = timeRanges[selectedDay].indexOf(selectedTimeRange);
            if (index !== -1) {
                timeRanges[selectedDay][index] = newTimeRange;
            }
            updateDisplay();
            document.getElementById('edit-popup').style.display = 'none';
            document.getElementById('overlay').style.display = 'none';
        }
        function selectTimeRange(element, day, range) {
            document.getElementById('overlay').style.display = 'block';
            selectedDay = day;
            selectedTimeRange = range;
            let [from, to] = selectedTimeRange.split(' - ');
            document.getElementById('edit-fromTime').value = from;
            document.getElementById('edit-toTime').value = to;
            document.getElementById('edit-popup').style.display = 'block';
        }
        function closePopup() {
            document.getElementById('popup').style.display = 'none';
            document.getElementById('edit-popup').style.display = 'none';
            document.getElementById('overlay').style.display = 'none';
        }
        document.body.addEventListener('dblclick', (event) => {
            let editPopup = document.getElementById('edit-popup');
            let addPopup = document.getElementById('popup');
            if ((editPopup.style.display === 'block') || (addPopup.style.display === 'block')) {
                if (!editPopup.contains(event.target) || !addPopup.contains(event.target)) {
                    closePopup();
                }
            }
        });
        function removeTimeRange(element, day, range) {
            let index = timeRanges[day].indexOf(range);
            if (index !== -1) {
                element.classList.add('remove-animation');
                setTimeout(() => {
                    timeRanges[day].splice(index, 1);
                    updateDisplay();
                }, 500);
            }
            event.stopPropagation();
        }
        function updateDisplay() {
            document.querySelectorAll('.time-slot').forEach((item, index) => {
                let day = index % 7;
                let timeRangesForDay = timeRanges[getDayName(day)];
                let innerHTML = '';
                if (timeRangesForDay) {
                    for (let range of timeRangesForDay) {
                        let [start, end] = range.split(' - ');
                        let [startHour, startMinute] = start.split(':').map(Number);
                        let [endHour, endMinute] = end.split(':').map(Number);
                        let totalStartMinutes = startHour * 60 + startMinute;
                        let totalEndMinutes = endHour * 60 + endMinute;

                        if (totalEndMinutes - totalStartMinutes < 60) {
                            totalEndMinutes = totalStartMinutes + 60;
                            endHour = Math.floor(totalEndMinutes / 60);
                            endMinute = totalEndMinutes % 60;
                            end = `${String(endHour).padStart(2, '0')}:${String(endMinute).padStart(2, '0')}`;
                        }
                        let totalMinutesInDay = 24 * 60;
                        let topOffset = (totalStartMinutes / totalMinutesInDay) * 100;
                        let height = ((totalEndMinutes - totalStartMinutes) / totalMinutesInDay) * 100;
                        innerHTML += `<div class="time-range" style="top: ${topOffset}%; height: ${height}%" onclick="selectTimeRange(this, '${getDayName(day)}', '${range}')">
                                            ${range}
                                            <div class="remove-button" onclick="removeTimeRange(this.parentNode, '${getDayName(day)}', '${range}')">x</div>
                                          </div>`;

                    }
                }
                item.innerHTML = innerHTML;
            });
            sendTimeRanges();
        }

        function getQueryParam(paramName) {
            var urlParams = new URLSearchParams(window.location.search);
            return urlParams.get(paramName);
        }

        function sendTimeRanges(outletNumber) {
            var xhr = new XMLHttpRequest();
            var outletNumber = getQueryParam("outlet");
            var params = "DataTR=" + encodeURIComponent(JSON.stringify(timeRanges)) + "&outletNum=" + outletNumber;
            xhr.open("POST", "/updateTimeRanges", true);
            xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            xhr.onreadystatechange = function () {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    console.log(xhr.responseText);
                }
            }
            xhr.send(params);
        }
    </script>

</body>

</html>
)=====";
