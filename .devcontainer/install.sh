apt update && apt upgrade -y
# Linting
apt install -y python3-pip
pip install cpplint
# Tests
apt install -y libboost-all-dev