install:
	mkdir /decoders || true
	find . -maxdepth 1 -type d -not -path '*/.*' -not -path '.' -exec bash -c "cd \"{}\" && make" \;