INCLUDE = -I$(LIB_PATH)/include \
		  -I$(LIB_PATH)/ode/src \
		  -I$(LIB_PATH)/ode/src/joints \
		  -I$(LIB_PATH)/OPCODE \
		  -I$(LIB_PATH)/ou/include

DEFINES = -DdIDESINGLE -DCCD_IDESINGLE -D_OU_NAMESPACE=odeou -DODE_LIB

SRC_DIRS = $(LIB_PATH)/ode/src \
		   $(LIB_PATH)/ode/src/joints \
		   $(LIB_PATH)/ode/OPCODE/Ice \
		   $(LIB_PATH)/ode/OPCODE \
		   $(LIB_PATH)/ou/src/ou