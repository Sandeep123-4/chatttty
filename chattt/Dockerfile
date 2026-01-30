# Use Ubuntu as base
FROM ubuntu:22.04

# Install g++ and build tools
RUN apt-get update && apt-get install -y g++ make

# Set working directory
WORKDIR /app

# Copy server code to container
COPY server.cpp .

# Compile the server
RUN g++ server.cpp -pthread -o server

# Use the port Render provides
ENV PORT 10000

# Expose port
EXPOSE $PORT

# Run server
CMD ["./server"]
