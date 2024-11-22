#!/usr/bin/env php

<?php
	$request_method = getenv("REQUEST_METHOD");

	echo "<html><body>";
	echo "<h1>PHP CGI Form</h1>";
	if ($request_method === 'POST') {
		$input = stream_get_contents(STDIN);
		parse_str($input, $query_params);
	} else {
		parse_str(getenv("QUERY_STRING"), $query_params);
	}

	$name = isset($query_params['name']) ? htmlspecialchars($query_params['name']) : null;
	$age = isset($query_params['age']) ? htmlspecialchars($query_params['age']) : null;

	if ($name && $age) {
		echo "<h2>Hello <u>$name</u>, you are <u>$age</u> years old!</h2>";
	} else {
		echo "<h2>Error: Missing name or age</h2>";
	}
	echo "</body></html>";

	// curl -X GET "http://localhost:5000/form?name=John&age=30"
	// curl -X POST -d "name=John&age=30" http://localhost:5000/form
?>


