
#pragma once

#include "Arduino.h"

// define web page
String webpage = R"(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        font-family: 
        Arial, sans-serif; 
        background-color: #f0f0f0; 
        padding: 5px;
      }
      h1 {
        color: #333; 
        text-align: center;
      }
      form {
        background-color: #fff; 
        max-width: 800px; 
        margin: 0 auto; 
        padding: 5px;  
        border-radius: 5px; 
        box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
      }
      label {
        display: block;  
        margin-bottom: 5px; 
        font-size: 16px; 
        color: #333;
      }
      input[type="text"], input[type="submit"] {
        width: 100%; 
        padding: 5px;  
        margin-bottom: 5px;  
        border-radius: 5px;  
        border: 1px solid #ccc; 
        font-size: 16px;
      }
      input[type="submit"] {
        background-color: #4CAF50; 
        color: white; 
        border: none; 
        cursor: pointer;
      }
      input[type="submit"]:hover {
        background-color: #45a049;
      }
    </style>
    <title>ESP32 Radio</title>
  </head>
 
  <body>
    <h1>ESP32 Radio - Sender einstellen</h1>
    <form action="/set">
    
      <table cellspacing=20px>
        <colgroup>
          <col width="30", padding=5px>
          <col width="150", padding=5px>
          <col width="510", padding=5px>
        </colgroup>

        <tr>
          <th>Sender Nr.</th>
          <th>Name</th>
          <th>Url</th>
        </tr>
        %s
      </table>
  
      <input type="submit" value="Speichern">
  
    </form>

  </body>

</html>
)";

