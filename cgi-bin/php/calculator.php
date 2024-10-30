#!/usr/bin/env php
<?php

function main() {
	header("Content-Type: application/json");

	// Get the input parameters
	$num1 = isset($_GET['num1']) ? (float)$_GET['num1'] : null;
	$num2 = isset($_GET['num2']) ? (float)$_GET['num2'] : null;
	$operation = isset($_GET['operation']) ? $_GET['operation'] : null;

    // Initialize the result
    $result = 0;

    // Perform the calculation based on the operation
    if ($num1 !== null && $num2 !== null && $operation !== null) {
        switch ($operation) {
            case 'add':
                $result = $num1 + $num2;
                break;
            case 'subtract':
                $result = $num1 - $num2;
                break;
            case 'multiply':
                $result = $num1 * $num2;
                break;
            case 'divide':
                if ($num2 != 0) {
                    $result = $num1 / $num2;
                } else {
                    echo json_encode(["error" => "Division by zero"]);
                    return;
                }
                break;
            default:
                echo json_encode(["error" => "Invalid operation"]);
                return;
        }
    } else {
        echo json_encode(["error" => "Missing parameters"]);
        return;
    }

    // Return the result as JSON
    echo json_encode(["result" => $result]);
}

main();
?>