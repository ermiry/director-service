# Test Director Service

### Development
```
sudo docker run \
  -it \
  --name director --rm \
  -p 5000:5000 --net worker \
  -v /home/ermiry/Documents/ermiry/Projects/director-service:/home/director \
  -e RUNTIME=development \
  -e PORT=5000 \
  -e CERVER_RECEIVE_BUFFER_SIZE=4096 -e CERVER_TH_THREADS=4 \
  -e CERVER_CONNECTION_QUEUE=4 \
  -e CONNECT_TO_REDIS=true -e REDIS_HOSTNAME=redis \
  ermiry/director-service:development /bin/bash
```

## Routes

#### GET /api/director
**Access:** Public \
**Description:** Director service top level route \
**Returns:**
  - 200 on success

#### GET /api/director/version
**Access:** Public \
**Description:** Returns director service current version \
**Returns:**
  - 200 on success

#### POST /api/director/transaction
**Access:** Public \
**Description:** A user has requested to create a new transaction \
**Returns:**
  - 200 on success creating transaction
  - 400 on failed to create new transaction
  - 401 on failed auth
  - 500 on server error
