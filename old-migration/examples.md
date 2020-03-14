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

**2. Diskless Migration:**
+ Just change the invokation to the migration script
```bash
sudo ./migration.py --name eureka --diskless
```

