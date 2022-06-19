FROM ubuntu:21.04

# Copying files
WORKDIR /usr/src/caging/

COPY . .

# To prevent hangs during tzdata package installation
ENV TZ=Europe/Stockholm
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Installing packages
RUN echo $(apt-get update)
RUN echo $(apt-get install -y $(cat dependencies.txt))

# Installing CGAL
WORKDIR ./CGAL-4.7
RUN echo $(cmake .)
RUN echo $(make)
RUN echo $(make install)
# update library cache to be able to find shared CGAL libraries
RUN ldconfig

# Building the non-gui version of 3D caging algorithm
WORKDIR /usr/src/caging/NCaging3D
RUN echo $(sh compile.sh)

# Building MoleQCage
WORKDIR /usr/src/caging/QMolecularCaging
RUN echo $(sh compile.sh)

# Copying non-gui binary to MoleQCage
RUN echo $(cp ../NCaging3D/bin/NCaging3D bin/)

WORKDIR /usr/src/caging/QMolecularCaging/bin
CMD ["./QMolecularCaging"]
