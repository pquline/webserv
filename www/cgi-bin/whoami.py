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
    <script>
        function deleteUserInfo() {{
            if (confirm('Are you sure you want to delete all your information? This action cannot be undone.')) {{
                fetch('/delete_user_info', {{
                    method: 'POST',
                    headers: {{
                        'Content-Type': 'application/json'
                    }}
                }})
                .then(response => {{
                    if (response.ok) {{
                        alert('Your information has been deleted successfully.');
                        window.location.href = '/editmyinfo.html';
                    }} else {{
                        throw new Error('Failed to delete information');
                    }}
                }})
                .catch(error => {{
                    alert('Error deleting your information. Please try again.');
                    console.error('Error:', error);
                }});
            }}
        }}
    </script>
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
        <div class="button-container">
            <a class="button" href="/index.html">Back to Home</a>
            <a class="button" onclick="deleteUserInfo()">Delete My Information</a>
        </div>
    </div>
</body>
</html>""")
