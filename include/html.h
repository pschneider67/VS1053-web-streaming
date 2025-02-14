
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
        padding: 2px;  
        border-radius: 4px; 
        box-shadow: 0px 0px 5px rgba(0, 0, 0, 0.1);
      }
      label {
        display: block;  
        margin-bottom: 2px; 
        font-size: 16px; 
        color: #333;
      }
      input[type="text"], input[type="submit"] {
        width: 100%; 
        padding: 2px;  
        margin-bottom: 2px;  
        border-radius: 4px;  
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
      
      table { width: 100%; border-collapse: collapse; }
      table, th, td { border: 1px solid black; }
      th, td { padding: 10px; text-align: center; }
      th { background-color: #4CAF50; color: white; }
      tr:nth-child(even) { background-color: #f2f2f2; }

    </style>
    <title>ESP32 Radio</title>
  </head>
 
  <body>
    <h1>ESP32 WebRadio - Sender einstellen</h1>
    <form action="/set">
    
      <table cellspacing=10px>
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

