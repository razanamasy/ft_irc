all:
	docker build -t ircserv .
	docker run -d -p 6667:6667 --name ircserv ircserv

status:
	@docker ${FLAGS} ps
	@docker ${FLAGS} images
	@docker container ls

start:
	docker container start ircserv

stop:
	docker container stop ircserv

rm_container:
	docker container rm ircserv

rm_img:
	docker rmi -f ircserv

clean: stop rm_container rm_img
