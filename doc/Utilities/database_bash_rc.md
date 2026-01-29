# .database.bash.rc

**Path:** `artdaq-database/Utilities/bashrc/.database.bash.rc`

**Purpose:** Bash shell configuration file that sets up the artdaq-database environment for interactive sessions. This script configures environment variables, sets up UPS products (Fermilab's product management system), and provides convenience functions for managing the web configuration editor server and SSH tunnels.

## Key Concepts

### UPS Product Setup

The script uses Fermilab's UPS (Unix Product Support) system to configure software products with specific versions and qualifiers. This ensures consistent software environments across different systems.

### Environment Variables

Key environment variables configure the database connection and logging:
- `ARTDAQ_DATABASE_HOME` - Base directory for all database-related files
- `ARTDAQ_DATABASE_URI` - Connection string for the database backend
- `ARTDAQ_DATABASE_LOGS` - Directory for log files

### Web GUI Server

The script provides functions to manage the Node.js-based web configuration editor server, which provides a graphical interface for managing configurations.

## Thread Safety

- **Thread Safety:** Shell script (single process execution)
- **Concurrent Access:** Multiple users can source independently; shared resources (server) require coordination

## Environment Variables

### Variables Set by Script

| Variable | Example Value | Description |
|----------|---------------|-------------|
| `ARTDAQ_DATABASE_HOME` | `${HOME}/daqsw` | Base directory for database files |
| `ARTDAQ_DATABASE_URI` | `filesystemdb://${ARTDAQ_DATABASE_HOME}/databases/filesystemdb/test_configuration_db` | Database connection string |
| `ARTDAQ_DATABASE_LOGS` | `${ARTDAQ_DATABASE_HOME}/logs/conftool` | Log file directory |

### Prerequisite Variables

The script expects these to be available:
- `HOME` - User's home directory (standard shell variable)
- UPS products setup in `/home/pduser/daqsw/products`

## UPS Products Configured

| Product | Version | Qualifiers | Description |
|---------|---------|------------|-------------|
| `artdaq` | v2_00_00 | e10:eth:prof:s41 | Main artdaq framework |
| `artdaq_database` | v1_04_02 | e10:prof:s41 | Configuration database library |
| `artdaq_node_server` | (commented) | prof:e10:s35 | Web GUI server (optional) |

**Note:** Versions and qualifiers in the template are examples and should be updated for your environment.

## Functions

### `startWebGUIServer()`

**Brief:** Starts the Node.js web configuration editor server in the background.

**Usage:**
```bash
startWebGUIServer
```

**Behavior:**
- Changes to `$ARTDAQ_NODE_SERVER_DIR`
- Runs `node serverbase.js` with nohup
- Logs output to `${ARTDAQ_DATABASE_LOGS}/nodeserver-{timestamp}-{hostname}.log`
- Returns to original directory

**Preconditions:**
- artdaq_node_server UPS product must be set up
- `$ARTDAQ_NODE_SERVER_DIR` must be defined

### `stopWebGUIServer()`

**Brief:** Stops all Node.js processes running under the current user.

**Usage:**
```bash
stopWebGUIServer
```

**Behavior:**
- Runs `killall -u sbnd -9 node`
- Forcefully terminates all node processes for user "sbnd"

**Warning:** This stops ALL node processes for the user, not just the web GUI server.

### `statusWebGUIServer()`

**Brief:** Shows the status of running web GUI server processes.

**Usage:**
```bash
statusWebGUIServer
```

**Behavior:**
- Lists processes matching `serverbase.js`
- Excludes grep and grid processes from output

### `guitunnel(node_server)`

**Brief:** Creates an SSH tunnel to access the web GUI on a remote DAQ node.

**Parameters:**
- `node_server` - Short hostname of the target DAQ node (without domain)

**Usage:**
```bash
guitunnel sbnd-daq01
# Then access: http://localhost:8080/db/client.html
```

**Behavior:**
- Allocates a random port in range 8800-8899
- Creates two-hop SSH tunnel through sbnd-daq00.fnal.gov
- Forwards local port 8080 to remote port 8080

**SSH Options Used:**
- `UserKnownHostsFile=/dev/null` - Don't save host keys
- `StrictHostKeyChecking=no` - Don't verify host keys

**Security Note:** These permissive SSH options are convenient but reduce security. Consider using stricter options in production.

## Usage Examples

### Initial Environment Setup

```bash
# Add to your .bashrc for automatic setup
source /path/to/.database.bash.rc

# Or source manually when needed
source /software/artdaq/config/.database.bash.rc
```

### Session Startup

```bash
# Source the configuration
source .database.bash.rc

# Script outputs:
# user@hostname uname-info
#
# artdaq v2_00_00 -f Linux64bit+3.10-2.17 -q e10:eth:prof:s41 -z /software/products
# artdaq_database v1_04_02 -f Linux64bit+3.10-2.17 -q e10:prof:s41 -z /software/products
#
# ARTDAQ_DATABASE_URI=filesystemdb:///home/user/daqsw/databases/filesystemdb/test_configuration_db
#
# conftool.sh [help output]
#
# Web GUI server pid: 12345
# Current dir: /home/user/daqsw/work-db-dir
```

### Web GUI Management

```bash
# Start the web GUI server
startWebGUIServer
# Output: logs to nodeserver-20240115-myhost.log

# Check if it's running
statusWebGUIServer
# Output: user 12345 ... node serverbase.js

# Access locally at: http://localhost:8080/db/client.html

# Stop the server
stopWebGUIServer
```

### Remote Access via SSH Tunnel

```bash
# Create tunnel to remote node
guitunnel daq-node01

# In another terminal, access the GUI at:
# http://localhost:8080/db/client.html

# The tunnel stays open until you press Ctrl+C
```

## Script Output

When sourced, the script displays:

1. User and system information
2. List of active UPS products (artdaq*)
3. Current ARTDAQ_DATABASE_URI value
4. conftool.sh help message
5. Web GUI server PID (if running)
6. Current working directory

## Customization Required

The following items should be customized for your environment:

| Item | Template Value | What to Change |
|------|----------------|----------------|
| Products path | `/home/pduser/daqsw/products` | Your UPS products location |
| artdaq version | v2_00_00, e10:eth:prof:s41 | Your installed version |
| artdaq_database version | v1_04_02, e10:prof:s41 | Your installed version |
| Database path | `${ARTDAQ_DATABASE_HOME}/databases/...` | Your database location |
| User in killall | sbnd | Your username |
| SSH hosts | sbnd-daq00.fnal.gov | Your gateway hosts |

## Directory Structure

Expected directory layout:

```
${ARTDAQ_DATABASE_HOME}/
  databases/
    filesystemdb/
      test_configuration_db/    # FileSystemDB database
  logs/
    conftool/                   # conftool log files
    nodeserver-*.log            # Web server logs
  work-db-dir/                  # Working directory
```

## Common Pitfalls

- **Wrong UPS Versions:** Ensure specified versions are actually installed
- **Path Issues:** All paths must exist or be creatable
- **SSH Keys:** guitunnel requires SSH key authentication
- **Port Conflicts:** Default ports (8080, 8800-8899) may conflict with other services
- **User Mismatch:** stopWebGUIServer kills processes for hardcoded user "sbnd"

## Security Considerations

### SSH Tunnel Options

The guitunnel function uses permissive SSH options for convenience:
- Host key checking disabled
- Known hosts not saved

For production environments, consider:
```bash
# More secure alternative
ssh -o "StrictHostKeyChecking=accept-new" \
    -o "UserKnownHostsFile=~/.ssh/known_hosts_daq" \
    ...
```

### Web Server Binding

The web GUI server binds to all interfaces by default. Consider restricting to localhost if only tunnel access is needed.

## Relationship to Other Components

- **conftool.sh:** Bash wrapper script displayed by this configuration
- **artdaq_node_server:** UPS product providing the web GUI
- **mongodbserver@.service:** May be running as the backend database

## See Also

- [systemd_services.md](./systemd_services.md) - Service management scripts
- [mongod_conf.md](./mongod_conf.md) - MongoDB configuration
- [conftool_py.md](./conftool_py.md) - Python configuration tool
