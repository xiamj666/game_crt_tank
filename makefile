#myGame by xiamj 2022-9-8 
GCC_TOOL=gcc
INCLUDE_LIB = pthread
#==================================找.h文件===================================
INCLUDE_PATH=-I. -I./inc
#==================================找.c文件===================================
#.c文件位置
SUBDIRS=./ ./src
#在文件夹中找.c文件
C_SRCS:=$(shell find $(SUBDIRS) -maxdepth 1 -name "*.c")
#将所有的.c替换成.o
SRC_FILE:=$(C_SRCS:.c=.o)  
$(info $(SRC_FILE))
#================================目标与依赖===================================
TARGET = myGame
all:$(TARGET)
#实现目标
$(TARGET):$(SRC_FILE)
	$(GCC_TOOL) -o $@ $(INCLUDE_PATH) -$(INCLUDE_LIB) $(SRC_FILE)
	rm -f $(SRC_FILE)
%.o:%.c
	$(GCC_TOOL) -c $(INCLUDE_PATH) -$(INCLUDE_LIB) $^ -o $@
#删除       
clean:
	rm -f $(SRC_FILE)
	rm -f $(TARGET_1)