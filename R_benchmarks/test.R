library(igraph)

N  <- 10000  
m0 <- 10     
m  <- 3      

t0 <- Sys.time()
g <- barabasi.game(N, m = m, directed = FALSE)  # BA graph
t1 <- Sys.time()
cat("Graph generated in", as.numeric(difftime(t1, t0, units="secs")), "seconds\n")

t0 <- Sys.time()
total_edges <- gsize(g)  # total edges
t1 <- Sys.time()
cat("Edge counting:", total_edges, "edges in",
    as.numeric(difftime(t1, t0, units="secs")), "seconds\n")

t0 <- Sys.time()
connected_count <- 0

for(i in 1:N){
  for(j in (i + 1):N){
  connected_count <- connected_count + are.connected(g, i, j)
  }
}
t1 <- Sys.time()
cat("Connectivity check on 1000 random pairs counted", connected_count, "connections in", as.numeric(difftime(t1, t0, units="secs")), "seconds\n")
