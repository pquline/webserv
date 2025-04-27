#!/usr/bin/env python3

try:
    with open("www/usrInfo.txt", "r") as f:
        lines = f.read().splitlines()
        firstname = lastname = school = "Unknown"
        for line in lines:
            if line.startswith("firstname:"):
                firstname = line.split(":", 1)[1].strip()
            elif line.startswith("lastname:"):
                lastname = line.split(":", 1)[1].strip()
            elif line.startswith("school:"):
                school = line.split(":", 1)[1].strip()
except FileNotFoundError:
    firstname = lastname = school = "test"

print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Who Am I?</title>
    <link rel="stylesheet" href="../styles.css">
</head>
<body>
    <div class="container">
        <h1>Who Am I?</h1>
		<div class="info-container">
			<div class="form-group">
				<label>First Name:</label>
				<p>{firstname}</p>
			</div>
			<div class="form-group">
				<label>Last Name:</label>
				<p>{lastname}</p>
			</div>
			<div class="form-group">
				<label>School:</label>
				<p>{school}</p>
			</div>
		</div>
        <a class="button" href="/index.html">Back to Home</a>
    </div>
</body>
</html>""")
