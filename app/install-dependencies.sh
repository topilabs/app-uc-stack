#!/bin/bash

# Install client dependencies
echo "Installing client dependencies..."
cd client
npm install

# Install server dependencies
echo "Installing server dependencies..."
cd ../server
npm install

echo "All dependencies installed successfully!"
