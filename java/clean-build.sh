#!/bin/sh

#rm -rf src/
rm -rf target/

mkdir target

# -XX:+AggressiveOpts -XX:+UseStringCache -XX:+OptimizeStringConcat -XX:+UseCompressedStrings
./jdk1.8.0_131/bin/javac -cp "./jars/fastutil-7.2.0.jar:./jars/rapidoid-essentials-5.3.4.jar:./jars/rapidoid-commons-5.3.4.jar:./jars/rapidoid-net-5.3.4.jar:./jars/rapidoid-buffer-5.3.4.jar:./jars/rapidoid-http-fast-5.3.4.jar:./trove-3.0.3.jar" -d target/ src/cz/cvut/fel/esw/server/*.java
echo "compiled"
./jdk1.8.0_131/bin/java -server -XX:+AggressiveOpts  \
-cp ./jars/jackson-dataformat-yaml-2.8.5.jar:./jars/fastutil-7.2.0.jar:./jars/snakeyaml-1.17.jar:./jars/jackson-annotations-2.8.0.jar:./jars/jackson-module-afterburner-2.8.5.jar:./jars/jackson-core-2.8.5.jar:./jars/jackson-databind-2.8.5.jar:./jars/javassist-3.20.0-GA.jar:./jars/rapidoid-essentials-5.3.4.jar:./jars/rapidoid-commons-5.3.4.jar:./jars/rapidoid-net-5.3.4.jar:./jars/rapidoid-buffer-5.3.4.jar:./jars/rapidoid-http-fast-5.3.4.jar:trove-3.0.3.jar:target/ cz.cvut.fel.esw.server.Main
