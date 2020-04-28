all: submission

submission: clean
	python3 ./make_submission.py \
	&& tar -cvf proj1_2.tar parsetools/ string/ symbol_list/

clean:
	rm -rf parsetools/ string/ symbol_list/
