#!/usr/bin/python
# Python script to show the value of a cookie from an environment variable

import os
from http.cookies import SimpleCookie

# Assuming the cookie is stored in an environment variable named 'COOKIE_HEADER'
cookie_header = os.environ.get('COOKIE_HEADER', '')

# Check if the cookie_header is not empty
if cookie_header:
    # Create a SimpleCookie object and load the cookie header into it
    cookie = SimpleCookie()
    cookie.load(cookie_header)

    # Extract and print the value of a specific cookie, e.g., 'id'
    if 'id' in cookie:
        print("Value of id cookie:", cookie['id'].value)
    else:
        print("Cookie 'id' not found in the cookie header.")
else:
    print("No cookie header found in environment variables.")
