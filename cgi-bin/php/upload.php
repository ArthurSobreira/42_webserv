#!/usr/bin/env php

<?php
	echo "<html><body>";
	echo "<h1>PHP CGI File Upload</h1>";
	if (!isset($_SERVER['CONTENT_TYPE']) || strpos($_SERVER['CONTENT_TYPE'], 'multipart/form-data') === false) {
		echo "<h2>Error: Content type is not multipart/form-data or missing. Received: " . htmlspecialchars($_SERVER['CONTENT_TYPE']) . "</h2>";
		exit;
	}

	$boundary = substr($_SERVER['CONTENT_TYPE'], strpos($_SERVER['CONTENT_TYPE'], "boundary=") + 9);
	$boundary = "--" . $boundary;

	$raw_data = stream_get_contents(STDIN);
	if (!$raw_data) {
		echo "<h2>Error: No data received. Check form enctype and field name.</h2>";
		exit;
	}

	echo "<h2>Raw Data:</h2><pre>" . htmlspecialchars($raw_data) . "</pre>";

	$parts = explode($boundary, $raw_data);
	array_pop($parts);

	foreach ($parts as $part) {
		$part = trim($part);
		if (empty($part)) continue;

		if (preg_match('/Content-Disposition: form-data; name="file"; filename="([^"]+)"/', $part, $matches)) {
			$filename = $matches[1];

			$file_data_start = strpos($part, "\r\n\r\n") + 4;
			$file_data = substr($part, $file_data_start, -2);

			$uploadPath = getenv('UPLOAD_PATH') ?: '/tmp';
			$filePath = $uploadPath . '/' . basename($filename);

			if (file_put_contents($filePath, $file_data)) {
				echo "<h2>File <u>{$filename}</u> uploaded successfully to <u>{$filePath}</u></h2>";
			} else {
				echo "<h2>Error: Could not save the uploaded file.</h2>";
			}
		}
	}
	echo "</body></html>";

	// curl -X POST -F "file=@/etc/passwd" http://localhost:5000/upload
?>
