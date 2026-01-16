# mongod.conf

**Path:** `artdaq-database/Utilities/config/mongod.conf`

**Purpose:** Template MongoDB configuration file for artdaq-database deployments. This YAML configuration file provides recommended settings for running MongoDB as a storage backend for DAQ configuration data. It should be customized for each deployment environment.

## Key Concepts

### MongoDB Configuration

MongoDB uses YAML configuration files to specify runtime settings. This template provides a starting point with sensible defaults for development and testing, with commented sections for production features.

### Path Customization

All paths in this template are examples that must be customized for your environment:
- Log file path
- Data directory path
- PID file path

### Security by Default

The template binds to localhost only (127.0.0.1) for security. This prevents remote access until explicitly configured.

## Thread Safety

- **Thread Safety:** Configuration file (read once at MongoDB startup)
- **Concurrent Access:** Not applicable (MongoDB manages its own concurrency)

## Configuration Sections

### systemLog

Configures MongoDB logging behavior:

```yaml
systemLog:
  destination: file           # Write logs to file (not syslog)
  logAppend: true            # Append to existing log file
  path: /path/to/mongod.log  # Log file location
```

| Option | Value | Description |
|--------|-------|-------------|
| `destination` | `file` | Write logs to a file |
| `logAppend` | `true` | Append to existing log (good for rotation) |
| `path` | `<path>` | Full path to the log file |

### storage

Configures data storage:

```yaml
storage:
  dbPath: /path/to/data      # Data directory
  journal:
    enabled: true            # Enable journaling for durability
```

| Option | Value | Description |
|--------|-------|-------------|
| `dbPath` | `<path>` | Directory for database files |
| `journal.enabled` | `true` | Enable write-ahead logging for crash recovery |

### processManagement

Configures process behavior:

```yaml
processManagement:
  fork: true                       # Run as daemon
  pidFilePath: /path/to/mongod.pid # PID file for process management
```

| Option | Value | Description |
|--------|-------|-------------|
| `fork` | `true` | Fork and run in background |
| `pidFilePath` | `<path>` | Location of PID file |

### net

Configures network settings:

```yaml
net:
  port: 27017                # MongoDB port
  bindIp: 127.0.0.1         # Listen address(es)
```

| Option | Value | Description |
|--------|-------|-------------|
| `port` | `27017` | Default MongoDB port |
| `bindIp` | `127.0.0.1` | Bind to localhost only for security |

## Commented Sections

The template includes placeholder sections for advanced features:

| Section | Purpose |
|---------|---------|
| `security` | Authentication and authorization settings |
| `operationProfiling` | Query profiling for performance analysis |
| `replication` | Replica set configuration for high availability |
| `sharding` | Sharded cluster configuration for horizontal scaling |
| `auditLog` | Enterprise audit logging |
| `snmp` | Enterprise SNMP monitoring |

## Usage Examples

### Development Configuration

```yaml
# Minimal development configuration
systemLog:
  destination: file
  logAppend: true
  path: /var/log/mongodb/mongod.log

storage:
  dbPath: /var/lib/mongodb
  journal:
    enabled: true

processManagement:
  fork: true
  pidFilePath: /var/run/mongodb/mongod.pid

net:
  port: 27017
  bindIp: 127.0.0.1
```

### Network-Accessible Configuration

```yaml
# Allow connections from DAQ network
net:
  port: 27017
  bindIp: 127.0.0.1,192.168.1.100

# Enable authentication for network access
security:
  authorization: enabled
```

### Production with Replica Set

```yaml
systemLog:
  destination: file
  logAppend: true
  path: /data/mongodb/logs/mongod.log

storage:
  dbPath: /data/mongodb/data
  journal:
    enabled: true
  wiredTiger:
    engineConfig:
      cacheSizeGB: 2  # Adjust based on available RAM

processManagement:
  fork: true
  pidFilePath: /data/mongodb/mongod.pid

net:
  port: 27017
  bindIp: 127.0.0.1,192.168.1.100

security:
  authorization: enabled
  keyFile: /data/mongodb/keyfile

replication:
  replSetName: rs0
```

### Tuned for Configuration Database

```yaml
storage:
  dbPath: /data/artdaq_db/mongodb
  journal:
    enabled: true
  wiredTiger:
    engineConfig:
      cacheSizeGB: 1        # 1GB cache sufficient for config data
    collectionConfig:
      blockCompressor: snappy  # Good compression for JSON documents

net:
  port: 28047                # Custom port to avoid conflicts
  bindIp: 127.0.0.1
```

## Directory Structure

The template expects this directory structure:

```
/home/pduser/daqsw/databases/
  mongodb/              # dbPath - data files
    WiredTiger/
    journal/
    collection-*.wt
    index-*.wt
  logs/
    mongod.log          # Log file
  mongod.pid            # PID file
```

## Starting MongoDB

```bash
# Using the configuration file
mongod --config /path/to/mongod.conf

# Or with systemd service
sudo systemctl start mongodbserver@mydb.service
```

## Security Considerations

### Default Security (Localhost Only)

The template binds to `127.0.0.1` only, which:
- Prevents remote access without explicit configuration
- Is safe for development environments
- Must be changed for multi-node deployments

### Production Security Recommendations

1. **Enable Authentication:**
   ```yaml
   security:
     authorization: enabled
   ```

2. **Use Keyfile for Replica Sets:**
   ```yaml
   security:
     keyFile: /secure/path/to/keyfile
   ```

3. **Restrict Network Access:**
   ```yaml
   net:
     bindIp: 127.0.0.1,<specific-trusted-ip>
   ```

4. **Consider TLS/SSL:**
   ```yaml
   net:
     tls:
       mode: requireTLS
       certificateKeyFile: /path/to/server.pem
   ```

## Performance Tuning

### WiredTiger Cache

Set cache size based on available RAM (default is 50% of RAM - 1GB):

```yaml
storage:
  wiredTiger:
    engineConfig:
      cacheSizeGB: 1  # For dedicated config database
```

### Connection Pool

For high-concurrency environments:

```yaml
net:
  maxIncomingConnections: 100
```

## Integration with Systemd

This configuration is designed to work with the `mongodbserver@.service` systemd unit:

```ini
[Service]
Type=forking
ExecStart=/data/artdaq_database/mongod-ctrl.sh start
```

Key compatibility points:
- `fork: true` matches systemd `Type=forking`
- `pidFilePath` enables process tracking

## Common Pitfalls

- **Path Permissions:** Ensure MongoDB user can write to all specified paths
- **Port Conflicts:** Default port 27017 may conflict with existing MongoDB instances
- **Memory Limits:** WiredTiger cache can consume significant RAM
- **Disk Space:** Journal and data files require adequate storage
- **SELinux/AppArmor:** May block non-standard paths

## Validation

MongoDB validates configuration on startup:

```bash
# Test configuration syntax
mongod --config /path/to/mongod.conf --configExpand none --validate

# Dry run (doesn't start server)
mongod --config /path/to/mongod.conf --setParameter validateConfig=1
```

## Relationship to Other Components

- **mongod-ctrl.sh:** Control script that starts MongoDB with additional options
- **mongodbserver@.service:** Systemd service unit that sources environment and runs mongod
- **artdaq-database StorageProviders:** MongoDB provider that connects to this server

## See Also

- [systemd_services.md](./systemd_services.md) - Service unit files for MongoDB
- [External: MongoDB Configuration Options](https://docs.mongodb.com/manual/reference/configuration-options/)
- [External: MongoDB Security Checklist](https://docs.mongodb.com/manual/administration/security-checklist/)
