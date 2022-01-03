CREATE TABLE computer_table (

    computer_id int NOT NULL AUTO_INCREMENT,
    system_name varchar(32) NOT NULL,
    host_name varchar(32),

    PRIMARY KEY (computer_id)
)

CREATE TABLE usage {
    usage_type int NOT NULL,
    current int,
    one_hour int,
    one_day int,
    one_month int,
    
}

// usage info will be dealt with using a circle queue