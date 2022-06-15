SRC_FILES=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRC_FILES))
TARGET=mycc

all: $(TARGET) pdf
#all: $(TARGET)

pdf:
	pdflatex -output-directory=../Documentation ../Documentation/Developer.tex

$(TARGET) : $(OBJS)
	@g++ -Wall -Werror -std=c++11 $^ -o $(TARGET)

%.o : %.cpp
	@g++  -Wall -g -Werror -std=c++11 -c $< -o $@

.phony:all

clean:
	@rm -f *.o ./*.error ./*.output ./*.txt $(TARGET) ../Documentation/Developer.aux ../Documentation/Developer.log ../Documentation/Developer.fls ../Documentation/Developer.pdf
