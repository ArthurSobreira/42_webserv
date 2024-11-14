#!/usr/bin/env php

<?php
    $request_method = getenv("REQUEST_METHOD");

    echo "<html><body>";
    echo "<h1>Php CGI Calculator</h1>";
    if ($request_method === 'GET') {
        parse_str(getenv("QUERY_STRING"), $query_params);

        try {
            $num1 = isset($query_params['num1']) ? floatval($query_params['num1']) : 0;
            $num2 = isset($query_params['num2']) ? floatval($query_params['num2']) : 0;
            $operation = isset($query_params['operation']) ? $query_params['operation'] : 'add';

            switch ($operation) {
                case 'add':
                    $result = $num1 + $num2;
                    break;
                case 'sub':
                    $result = $num1 - $num2;
                    break;
                case 'mul':
                    $result = $num1 * $num2;
                    break;
                case 'div':
                    if ($num2 != 0) {
                        $result = $num1 / $num2;
                    } else {
                        $result = 'Error: Division by zero';
                    }
                    break;
                default:
                    $result = 'Invalid Operation';
                    break;
            }
            echo "<h2>Result: $result</h2>";

        } catch (Exception $e) {
            echo "<h2>Error: Invalid number</h2>";
        }
    } else {
        echo "<h2>Error: Invalid request method</h2>";
    }
    echo "</body></html>";

    // curl -X GET "http://localhost:5000/calculator?num1=10&num2=20&operation=add"
?>
