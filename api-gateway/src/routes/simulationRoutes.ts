// api-gateway/src/routes/simulationRoutes.ts
import { Router } from 'express';
import {
  createSimulation,
  getSimulations,
  getSimulationById,
  cancelSimulation,
  getTopologyTemplates,
  getWorkloadPatterns
} from '../controllers/simulationController';
import { authenticateToken } from '../middleware/authMiddleware';

const router = Router();

// All simulation routes require authentication
router.use(authenticateToken);

/**
 * @route   GET /api/v1/simulations/templates/topologies
 * @desc    Get available topology templates
 * @access  Private
 */
router.get('/templates/topologies', getTopologyTemplates);

/**
 * @route   GET /api/v1/simulations/templates/workloads
 * @desc    Get available workload patterns
 * @access  Private
 */
router.get('/templates/workloads', getWorkloadPatterns);

/**
 * @route   POST /api/v1/simulations
 * @desc    Create a new simulation job
 * @access  Private
 * @body    { name, description?, topologyId, workloadId, simulationTime?, ... }
 */
router.post('/', createSimulation);

/**
 * @route   GET /api/v1/simulations
 * @desc    Get simulation jobs for authenticated user
 * @access  Private
 * @query   { page?, limit?, status?, sortBy?, sortOrder? }
 */
router.get('/', getSimulations);

/**
 * @route   GET /api/v1/simulations/:id
 * @desc    Get specific simulation job details
 * @access  Private
 * @params  id - simulation job UUID
 */
router.get('/:id', getSimulationById);

/**
 * @route   DELETE /api/v1/simulations/:id
 * @desc    Cancel a simulation job
 * @access  Private
 * @params  id - simulation job UUID
 */
router.delete('/:id', cancelSimulation);

export default router;
