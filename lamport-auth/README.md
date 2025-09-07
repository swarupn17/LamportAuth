# Initial Hash Generation
1. cd tools/gen_chain

# preferred, use Homebrew prefix
PREFIX=$(brew --prefix)

clang++ -std=c++17 gen_chain.cpp -I${PREFIX}/include -L${PREFIX}/lib -lcryptopp -o gen_chain \
  -Wl,-search_paths_first -Wl,-headerpad_max_install_names

2. ./gen_chain "random_seed_123" 100


# Terminal 1 (Alice)
./lamport-app --config config_alice.json

# Terminal 2 (Bob)
./lamport-app --config config_bob.json


./build/config_test

./build/lamport_app
