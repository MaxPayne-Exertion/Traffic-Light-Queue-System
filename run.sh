#!/bin/bash


echo "======================================"
echo "Traffic Light Queue System"
echo "======================================"
echo ""

# Check if programs are compiled
if [ ! -f "traf" ] || [ ! -f "sim" ]; then
    echo "Programs not found. Compiling..."
    make all
    if [ $? -ne 0 ]; then
        echo "Compilation failed!"
        exit 1
    fi
    echo "Compilation successful!"
    echo ""
fi

# Clean old lane files
echo "Cleaning old lane files..."
rm -f lanea.txt laneb.txt lanec.txt laned.txt
touch lanea.txt laneb.txt lanec.txt laned.txt


# Start traffic generator in background
./traf &
GENERATOR_PID=$!
echo "Traffic Generator started (PID: $GENERATOR_PID)"

# Give generator time to create some initial vehicles
sleep 2

# Start simulator in foreground

./sim

# Cleanup: kill generator when simulator stops
echo ""
echo "Stopping traffic generator..."
kill $GENERATOR_PID 2>/dev/null

echo "Simulation stopped."
