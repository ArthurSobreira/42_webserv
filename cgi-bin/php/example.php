<?php
// Get the query parameters
$num1 = isset($_GET['num1']) ? (int)$_GET['num1'] : 0;
$num2 = isset($_GET['num2']) ? (int)$_GET['num2'] : 0;
$operation = isset($_GET['operation']) ? $_GET['operation'] : 'add';

// Perform the operation
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
			$result = 'Error: Division by zero';
		}
		break;
	default:
		$result = 'Error: Invalid operation';
		break;
}

header('Content-Type: application/json');
echo json_encode(array('result' => $result));
?>
