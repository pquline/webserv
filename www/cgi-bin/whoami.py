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
    <title>Who Am I</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #f5f7fa;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }}
        .card {{
            background-color: white;
            padding: 2rem 3rem;
            border-radius: 15px;
            box-shadow: 0 8px 20px rgba(0, 0, 0, 0.1);
            text-align: center;
        }}
        .card h1 {{
            color: #333;
            margin-bottom: 1rem;
        }}
        .card p {{
            font-size: 1.1rem;
            margin: 0.5rem 0;
        }}
        .card a {{
            display: inline-block;
            margin-top: 1.5rem;
            text-decoration: none;
            color: #fff;
            background-color: #4A90E2;
            padding: 0.6rem 1.2rem;
            border-radius: 8px;
            transition: background-color 0.3s ease;
        }}
        .card a:hover {{
            background-color: #357ABD;
        }}
    </style>
</head>
<body>
    <div class="card">
        <h1>Who Am I?</h1>
        <p><strong>First Name:</strong> {firstname}</p>
        <p><strong>Last Name:</strong> {lastname}</p>
        <p><strong>School:</strong> {school}</p>
        <a href="/index.html">Back to Home</a>
    </div>
</body>
</html>""")
