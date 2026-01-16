# Systemd Services and Scripts

**Path:** `artdaq-database/Utilities/systemd/`

**Purpose:** Collection of systemd service unit files and shell scripts for managing artdaq-database infrastructure components. These files enable automated startup, monitoring, and management of MongoDB database servers, web configuration editor, SSH tunnels, and database backup operations.

## Key Concepts

### Template Unit Files

Systemd template units (ending in `@.service`) allow a single service definition to manage multiple instances. The instance name is passed via `%i` and used to locate configuration files.

Example: `mongodbserver@experiment_v4x_db.service` uses `experiment_v4x_db` as the instance name.

### Environment Files

Each service instance reads environment variables from a dedicated file:
- MongoDB: `/data/artdaq_database/{instance}/mongod.env`
- Web Editor: `/data/artdaq_database/{instance}/webconfigeditor.env`

### Control Scripts

Shell scripts (`*-ctrl.sh`) handle the actual start/stop logic, including:
- Setting up UPS products
- Creating directories
- Managing PID files
- Logging configuration

## Thread Safety

- **Thread Safety:** Services run as separate processes
- **Concurrent Access:** Multiple instances can run with different names
- **Locking:** PID and lock files prevent duplicate instances

## Service Unit Files

### mongodbserver@.service

**Purpose:** Manages MongoDB server instances for storing configuration data.

**Installation:**
```bash
# 1. Copy to systemd directory
sudo cp mongodbserver@.service /etc/systemd/system/

# 2. Create environment file
cat > /data/artdaq_database/experiment_v4x_db/mongod.env << EOF
export MONGOD_DATABASE_NAME=experiment_v4x_db
export MONGOD_BASE_DIR=/data/artdaq_database
export MONGOD_UPS_VER=v4_4_0
export MONGOD_UPS_QUAL=e20
export MONGOD_PORT=27017
EOF

# 3. Reload systemd and enable
sudo systemctl daemon-reload
sudo systemctl enable mongodbserver@experiment_v4x_db.service
sudo systemctl start mongodbserver@experiment_v4x_db.service
```

**Configuration:**

| Setting | Value | Description |
|---------|-------|-------------|
| Type | forking | Process forks to background |
| User/Group | artdaq/artdaq | Run as artdaq user |
| Restart | always | Auto-restart on failure |
| RestartSec | 300 | Wait 5 minutes before restart |
| TimeoutSec | 60 | Startup/shutdown timeout |

### mongodbarbiter@.service

**Purpose:** Manages MongoDB arbiter instances for replica set voting without data storage.

**Usage:** Same as mongodbserver, but uses `start_arbiter`/`stop_arbiter` commands.

**Port:** Arbiter runs on `MONGOD_PORT + 1`

### webconfigeditor@.service

**Purpose:** Manages the Node.js web configuration editor application.

**Installation:**
```bash
sudo cp webconfigeditor@.service /etc/systemd/system/

cat > /data/artdaq_database/experiment_v4x_db/webconfigeditor.env << EOF
export MONGOD_DATABASE_NAME=experiment_v4x_db
export MONGOD_BASE_DIR=/data/artdaq_database
export WEBEDITOR_UPS_VER=v1_02_03
export WEBEDITOR_UPS_QUAL=e20:prof
export WEBEDITOR_BASE_PORT=8080
export ARTDAQ_DATABASE_URI="mongodb://user:pass@host:27017/db?replicaSet=rs0"
EOF

sudo systemctl daemon-reload
sudo systemctl enable webconfigeditor@experiment_v4x_db.service
sudo systemctl start webconfigeditor@experiment_v4x_db.service
```

**Configuration:**

| Setting | Value | Description |
|---------|-------|-------------|
| User/Group | experimentdaq/experiment | Run as experiment user |
| Restart | always | Auto-restart on failure |
| RestartSec | 120 | Wait 2 minutes before restart |
| PIDFile | `/data/artdaq_database/%i/var/tmp/webconfigeditor.pid` | Process ID file |

### ssh-tunnel@.service

**Purpose:** Maintains persistent SSH tunnels for remote database access.

**Installation:**
```bash
sudo cp ssh-tunnel@.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable ssh-tunnel@experimentdaq01.service
sudo systemctl start ssh-tunnel@experimentdaq01.service
```

The instance name (`experimentdaq01`) is the hostname to tunnel to.

## Control Scripts

### mongod-ctrl.sh

**Purpose:** Control script for MongoDB server lifecycle management.

**Commands:**

| Command | Description |
|---------|-------------|
| `start` | Start MongoDB with replica set configuration |
| `stop` | Gracefully stop MongoDB |
| `restart` | Stop then start |
| `status` | Check if MongoDB is running |
| `configure` | Start in standalone mode (no replica set) |
| `start_arbiter` | Start arbiter instance |
| `stop_arbiter` | Stop arbiter instance |
| `status_arbiter` | Check arbiter status |

**Required Environment Variables:**

| Variable | Description |
|----------|-------------|
| `MONGOD_DATABASE_NAME` | Instance name |
| `MONGOD_BASE_DIR` | Base directory for database files |
| `MONGOD_UPS_VER` | MongoDB UPS product version |
| `MONGOD_UPS_QUAL` | MongoDB UPS qualifiers |
| `MONGOD_PORT` | Port number to listen on |

**Optional Environment Variables:**

| Variable | Description |
|----------|-------------|
| `MONGOD_BINDIP` | Additional IP addresses to bind (auto-detected if not set) |
| `MONGOD_NUMA_CTRL` | NUMA control command prefix |

**MongoDB Options Applied:**

```bash
mongod \
  --dbpath=${MONGOD_DATA_DIR} \
  --pidfilepath=${MONGOD_PID} \
  --port=${MONGOD_PORT} \
  --wiredTigerCacheSizeGB=1 \
  --bind_ip=127.0.0.1${MONGOD_BINDIP} \
  --logpath=${MONGOD_LOG} \
  --logappend \
  --fork \
  --replSet rs0 \
  --keyFile ${MONGOD_KEY}
```

**Auto-Generated Files:**
- Keyfile: `${MONGOD_BASE_DIR}/${MONGOD_DATABASE_NAME}/mongod.keyfile` (created if missing)
- Data directory: `data-{hostname}/`
- Log directory: `logs-{hostname}/`

### webconfigeditor-ctrl.sh

**Purpose:** Control script for web configuration editor lifecycle management.

**Commands:**

| Command | Description |
|---------|-------------|
| `start` | Start the Node.js web server |
| `stop` | Stop the web server |
| `restart` | Stop then start (also kills all node processes) |
| `status` | Check server status and test HTTP endpoint |
| `keeprunning` | Health check - restart if not responding |

**Required Environment Variables:**

| Variable | Description |
|----------|-------------|
| `MONGOD_DATABASE_NAME` | Instance name |
| `MONGOD_BASE_DIR` | Base directory |
| `WEBEDITOR_UPS_VER` | artdaq_node_server version |
| `WEBEDITOR_UPS_QUAL` | artdaq_node_server qualifiers |
| `WEBEDITOR_BASE_PORT` | HTTP port number |
| `ARTDAQ_DATABASE_URI` | Database connection string |

**Health Check:**

The `keeprunning` command tests the HTTP endpoint:
```bash
curl http://${WEBEDITOR_BINDIP}:${WEBEDITOR_BASE_PORT}/db/client.html
```
If the check fails, the server is automatically restarted.

### backup_artdaq_database.sh

**Purpose:** Automated database backup script for cron-based scheduling.

**Usage:**
```bash
# Run manually
./backup_artdaq_database.sh experiment_v4x_db

# Add to crontab for daily backup at 1:10 AM
crontab -e
# Add: 10 01 * * * /data/artdaq_database/backup_artdaq_database.sh experiment_v4x_db >> /data/artdaq_database/backup.log 2>&1
```

**Backup Methods:**
1. `conftool.py exportDatabase` - Logical backup to JSON files
2. `mongodump --gzip` - Physical backup of MongoDB data

**Backup Locations:**
- Primary: `/data/artdaq_database/{instance}/backup/{timestamp}/`
- Secondary: `/software/backup/experiment_v4x_db/` (via rsync)

**Databases Backed Up:**
- `experiment_db` - Main configuration database
- `experiment_db_archive` - Archived run configurations

### setup_database.sh

**Purpose:** Interactive shell setup script for database operations.

**Usage:**
```bash
source setup_database.sh
```

**Features:**
- Sets up artdaq_database UPS product
- Configures `ARTDAQ_DATABASE_URI` for MongoDB
- Provides bash completion for conftool.py
- Lists available configurations on startup

**Bash Completion:**

Tab-completion for conftool.py operations:
- `exportConfiguration` - completes with available configurations
- `getListOfAvailableRunConfigurations` - completes with prefixes
- `importConfiguration` - completes with prefixes

### ssh-tunnel-ctr.sh

**Purpose:** Creates persistent SSH tunnel for remote database access.

**Usage:**
```bash
./ssh-tunnel-ctr.sh experimentdaq01
```

**SSH Options:**
- Forwards port 28047 from remote to local
- Uses key-based authentication
- Keepalive: interval 60s, max 3 failures
- Logs to `/tmp/ssh-tunnel-{hostname}.log`

### initd_functions

**Purpose:** Library of shell functions for init.d-style service management.

**Key Functions:**

| Function | Description |
|----------|-------------|
| `daemon()` | Start a daemon process with options |
| `killproc()` | Stop a process gracefully, then forcefully |
| `status()` | Check process status |
| `pidofproc()` | Find PID of a process |
| `checkpid()` | Check if PID(s) are running |
| `success()` / `failure()` | Display colored status messages |
| `action()` | Run command and show success/failure |

**Usage in Control Scripts:**
```bash
source /data/artdaq_database/initd_functions

# Check process status
status -p /path/to/pidfile processname

# Display success message
success "Operation completed"
```

### Helper Scripts

#### start.sh

Starts both MongoDB server and arbiter:
```bash
#!/bin/bash
/usr/bin/systemctl start mongodbarbiter@experiment_v4x_db.service
/usr/bin/systemctl start mongodbserver@experiment_v4x_db.service
```

#### status.sh

Shows status of MongoDB services:
```bash
#!/bin/bash
/usr/bin/systemctl status mongodbarbiter@experiment_v4x_db.service
/usr/bin/systemctl status mongodbserver@experiment_v4x_db.service
```

#### redeploy.sh

Stops, reinstalls, and restarts MongoDB services:
```bash
#!/bin/bash
# Stops services
# Disables services
# Removes old unit files
# Copies new unit files
# Enables and starts services
```

## Directory Structure

```
/data/artdaq_database/
  mongod-ctrl.sh
  webconfigeditor-ctrl.sh
  backup_artdaq_database.sh
  ssh-tunnel-ctr.sh
  initd_functions

  experiment_v4x_db/
    mongod.env                    # MongoDB environment
    webconfigeditor.env           # Web editor environment
    mongod.keyfile                # Replica set authentication key
    data-hostname/                # MongoDB data directory
    logs-hostname/                # MongoDB log directory
    logs/                         # Web editor logs
    webconfigeditor/              # Web editor data
    var/tmp/                      # PID files
    backup/                       # Backup directory
      20240115120000/
        experiment_db/
        experiment_db_archive/

/etc/systemd/system/
  mongodbserver@.service
  mongodbarbiter@.service
  webconfigeditor@.service
  ssh-tunnel@.service
```

## Environment File Templates

### mongod.env

```bash
export MONGOD_DATABASE_NAME=experiment_v4x_db
export MONGOD_BASE_DIR=/data/artdaq_database
export MONGOD_UPS_VER=v4_4_0
export MONGOD_UPS_QUAL=e20
export MONGOD_PORT=28047
# Optional
export MONGOD_BINDIP=192.168.1.100
export MONGOD_NUMA_CTRL="numactl --interleave=all"
```

### webconfigeditor.env

```bash
export MONGOD_DATABASE_NAME=experiment_v4x_db
export MONGOD_BASE_DIR=/data/artdaq_database
export WEBEDITOR_UPS_VER=v1_02_03
export WEBEDITOR_UPS_QUAL=e20:prof
export WEBEDITOR_BASE_PORT=8080
export ARTDAQ_DATABASE_URI="mongodb://user:pass@192.168.1.1:28047,192.168.1.2:28047/experiment_db?replicaSet=rs0&authSource=admin"
```

## Usage Examples

### Complete MongoDB Setup

```bash
# 1. Create directory structure
mkdir -p /data/artdaq_database/mydb

# 2. Create environment file
cat > /data/artdaq_database/mydb/mongod.env << EOF
export MONGOD_DATABASE_NAME=mydb
export MONGOD_BASE_DIR=/data/artdaq_database
export MONGOD_UPS_VER=v4_4_0
export MONGOD_UPS_QUAL=e20
export MONGOD_PORT=27017
EOF

# 3. Install and start service
sudo cp mongodbserver@.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable mongodbserver@mydb.service
sudo systemctl start mongodbserver@mydb.service

# 4. Verify
sudo systemctl status mongodbserver@mydb.service
```

### Replica Set Initialization

```bash
# Start in configure mode (standalone)
mongod-ctrl.sh configure

# Connect and initialize replica set
mongo --port 27017
rs.initiate({
  _id: "rs0",
  members: [
    {_id: 0, host: "node1:27017"},
    {_id: 1, host: "node2:27017"},
    {_id: 2, host: "node3:27018", arbiterOnly: true}
  ]
})

# Stop and restart with replication
mongod-ctrl.sh stop
mongod-ctrl.sh start
```

### Automated Daily Backup

```bash
# Add to crontab
crontab -e

# Daily backup at 1:10 AM
10 01 * * * /data/artdaq_database/backup_artdaq_database.sh experiment_v4x_db >> /data/artdaq_database/backup.log 2>&1
```

## Troubleshooting

| Issue | Possible Cause | Solution |
|-------|---------------|----------|
| Service won't start | Missing environment file | Create required .env file |
| MongoDB connection refused | Wrong MONGOD_PORT or MONGOD_BINDIP | Check environment variables |
| Web editor not accessible | Wrong WEBEDITOR_BASE_PORT | Verify port and firewall settings |
| SSH tunnel fails | SSH keys not configured | Set up passwordless SSH |
| Backup fails | Disk full | Check disk space and clean old backups |

## Security Considerations

- **Keyfiles:** Automatically generated with 400 permissions (owner read-only)
- **SSH Tunnels:** Use key-based authentication only
- **Database Passwords:** Store securely, not in plain text
- **Network Binding:** Default to localhost; explicitly configure external access
- **Firewall:** Ensure only authorized hosts can connect

## Relationship to Other Components

- **mongod.conf:** Template configuration that control scripts extend
- **conftool.py:** Uses the database services for configuration management
- **artdaq_node_server:** UPS product providing web editor functionality

## See Also

- [mongod_conf.md](./mongod_conf.md) - MongoDB configuration template
- [database_bash_rc.md](./database_bash_rc.md) - Interactive shell setup
- [conftool_py.md](./conftool_py.md) - Python configuration tool
- [External: systemd Documentation](https://www.freedesktop.org/software/systemd/man/)
