# Load igraph library
if(!require(igraph)) install.packages("igraph", dependencies=TRUE)
library(igraph)

# Parameters
num_vertices <- 15000
connection_prob <- 0.5

# Measure graph generation time
cat("Generating Erdős–Rényi graph with", num_vertices, "vertices and p =", connection_prob, "...\n")
start_time_gen <- Sys.time()
g <- erdos.renyi.game(n = num_vertices, p.or.m = connection_prob, type = "gnp", directed = FALSE)
end_time_gen <- Sys.time()
gen_time <- end_time_gen - start_time_gen

cat("Graph generated in", round(as.numeric(gen_time, units="secs"), 3), "seconds.\n")
cat("Number of vertices:", vcount(g), "\n")
cat("Number of edges:", ecount(g), "\n")

# Measure clustering coefficient computation time
start_time_clust <- Sys.time()
global_clustering <- transitivity(g, type = "global")
avg_local_clustering <- transitivity(g, type = "average")
end_time_clust <- Sys.time()
clust_time <- end_time_clust - start_time_clust

cat("Global clustering coefficient:", global_clustering, "\n")
cat("Average local clustering coefficient:", avg_local_clustering, "\n")
cat("Clustering coefficient computation took", round(as.numeric(clust_time, units="secs"), 3), "seconds.\n")
