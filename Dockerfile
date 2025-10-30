# Stage 1: Build stage
FROM alpine:latest AS builder

# Install build dependencies
RUN apk add --no-cache gcc musl-dev make

WORKDIR /app

# Copy source code
COPY . .

# Build the server
RUN make

# Stage 2: Runtime stage
FROM alpine:latest

WORKDIR /app

# Copy the compiled executable from the builder stage
COPY --from=builder /app/bin/mar1server /app/server

# Copy your static web files
COPY --from=builder /app/www /app/www

# Expose the port
EXPOSE 9001

# Run the server
CMD ["./server", "9001"]