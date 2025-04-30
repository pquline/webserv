#!/bin/bash

# Set the content type to HTML
# echo "Content-Type: text/html"
# echo ""

# Output the HTML
cat <<'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Digital Clock</title>
	<link rel="stylesheet" href="../styles.css">
    <style>

        .digital-clock {
            font-family: 'Courier New', monospace;
            font-size: 5rem;
            font-weight: bold;
            color: #40084e;
            text-shadow: 0 0 10px rgba(255, 255, 255, 0.5);
            margin: 20px 0;
            letter-spacing: 5px;
        }

        .date {
            font-size: 1.5rem;
            color: #333;
            margin-bottom: 30px;
        }
    </style>
</head>
<body>
    <div class="container">
		<h1>Digital Clock</h1>
		<div class="date" id="date"></div>
		<div class="digital-clock" id="clock"></div>
		<div class="button-container-2">
			<a class="button" href="/index.html">Back to Home</a>
		</div>
    </div>

    <script>
        function updateClock() {
            const now = new Date();

            // Update time
            const hours = String(now.getHours()).padStart(2, '0');
            const minutes = String(now.getMinutes()).padStart(2, '0');
            const seconds = String(now.getSeconds()).padStart(2, '0');
            document.getElementById('clock').textContent = `${hours}:${minutes}:${seconds}`;

            // Update date
            const options = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' };
            document.getElementById('date').textContent = now.toLocaleDateString(undefined, options);
        }

        // Update immediately and then every second
        updateClock();
        setInterval(updateClock, 1000);
    </script>
</body>
</html>
EOF
