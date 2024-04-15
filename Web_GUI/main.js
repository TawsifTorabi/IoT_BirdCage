    //var websocketAddress = "ws://192.168.247.69:80/ws";
        var websocketAddress = "ws://192.168.0.103:80/ws";
        var websocket;

        function connectWebSocket() {
            websocket = new WebSocket(websocketAddress);
            websocket.onopen = function() {
                console.log("WebSocket connected");
            };

            websocket.onmessage = function(event) {
                var data = event.data;
                parseData(data);
            };

            websocket.onclose = function() {
                console.log("WebSocket disconnected. Reconnecting...");
                setTimeout(connectWebSocket, 2000); // Attempt to reconnect after 2 seconds
            };
        }

        function parseData(data) {
            var lines = data.split("\n");
            for (var i = 0; i < lines.length; i++) {
                var line = lines[i].trim();
                if (line.startsWith("HX711:")) {
                    var hxData = line.split("|");
                    document.getElementById("hx711Data").innerText = hxData[0];
                    var readingIndex = line.indexOf("one reading:");
                    if (readingIndex !== -1) {
                        var weight = line.substring(readingIndex + 12).trim();
                        document.getElementById("weight").innerText = "Weight: " + weight;
                    }
                } else if (line.startsWith("PIR:")) {
                    var pirData = line.split(":");
                    document.getElementById("pirData").innerText = pirData[1].trim();
                } else if (line.startsWith("BMP280:")) {
                    var bmpData = line.split("|");
                    document.getElementById("temperature").innerText = bmpData[0].split("=")[1].trim();
                    document.getElementById("pressure").innerText = bmpData[1].split("=")[1].trim();
                    document.getElementById("altitude").innerText = bmpData[2].split("=")[1].trim();
                }
            }
        }

        connectWebSocket();
