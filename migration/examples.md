=====
Local Examples
=====

**1. Non-Diskless Migration:**
+ Run the following commands as root.
+ First run a test application (REDIS db by default)
```bash
./run.sh
```
+ Then perform a migration relying on the `tmp` filesystem:
```bash
sudo ./migration.py --name eureka
```

